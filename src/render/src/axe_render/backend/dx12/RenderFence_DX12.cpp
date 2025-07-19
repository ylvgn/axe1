#if AXE_RENDER_HAS_DX12

#include "RenderFence_DX12.h"
#include "RenderDevice_DX12.h"

namespace axe  {

RenderFence_DX12::RenderFence_DX12(CreateDesc& desc)
	: Base(desc)
	, _lastCompletedValue(desc.initialFenceValue)
	, _curSignal(desc.initialFenceValue + 1)
	, _lastSignaled(0)
{
	AXE_ASSERT(desc.device != nullptr);

	::HRESULT hr;

	auto* device	= static_cast<RenderDevice_DX12*>(desc.device);
	auto* d3dDevice = device->d3dDevice();

	hr = d3dDevice->CreateFence(desc.initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_d3dFence.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	Util::setDebugName(_d3dFence.ptr(), desc.debugName);

	_onGpuCompletedEvent = CreateEvent(nullptr, false, false, L"DX12 Fence Event");
	if (!_onGpuCompletedEvent) AXE_THROW();
}

RenderFence_DX12::~RenderFence_DX12() {
	if (_onGpuCompletedEvent) CloseHandle(_onGpuCompletedEvent);
}

bool RenderFence_DX12::onCheckCompleted() {
	if (!_d3dFence) AXE_THROW();
	return _d3dFence->GetCompletedValue() >= _lastSignaled; // true means gpu is done, false means cpu may keep waiting gpu.
}

void RenderFence_DX12::cpuWait(UINT64 expectGpuCompletedValue) {
	if (onCheckCompleted()) {
		// lock-free checking
		return;
	}

	auto scopedLock = ScopedLock_make(_fenceWaitCS);

	// Double-check after acquiring the lock (avoid race condition between threads)
	if (onCheckCompleted()) {
		// locked checking
		return;
	}

	auto hr = _d3dFence->SetEventOnCompletion(expectGpuCompletedValue, _onGpuCompletedEvent);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	DWORD result = WaitForSingleObject(_onGpuCompletedEvent, INFINITE); // CPU thread is blocking here until the GPU signals the fence. (INFINITE is Wait Forever)

	if (result == WAIT_OBJECT_0)
	{
		// finished
	}
}

void RenderFence_DX12::cpuWait() {
	cpuWait(_lastSignaled);
}

void RenderFence_DX12::_gpuWait(::ID3D12CommandQueue* d3dCmdQueue) {
	::HRESULT hr = d3dCmdQueue->Wait(_d3dFence, _curSignal);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);
}

UINT64 RenderFence_DX12::_gpuSignal(::ID3D12CommandQueue* d3dCmdQueue) {
	::HRESULT hr = d3dCmdQueue->Signal(_d3dFence, _curSignal);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);
	_lastSignaled = _curSignal;
	_curSignal++;
	return _lastSignaled;
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12