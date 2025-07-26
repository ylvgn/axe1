#if AXE_RENDER_HAS_DX12

#include "Fence_DX12.h"
#include "Device_DX12.h"

namespace axe  {

const TypeInfo* Fence_DX12::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "Fence_DX12";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

void Fence_DX12::onCreate(CreateDesc& desc) {
	_lastCompletedValue = desc.initialFenceValue;
	_curSignal			= desc.initialFenceValue + 1;
	_lastSignaled		= 0;

	::HRESULT hr;

	auto* device	= static_cast<Device_DX12*>(_device);
	auto* d3dDevice = device->d3dDevice();

	hr = d3dDevice->CreateFence(desc.initialFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_d3dFence.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	Util::setDebugName(_d3dFence.ptr(), "Fence_DX12");

	_onGpuCompletedEvent = ::CreateEvent(nullptr, false, false, L"DX12 Fence Event");

	if (!_onGpuCompletedEvent)
		AXE_THROW();
}

Fence_DX12::~Fence_DX12() {
	if (_onGpuCompletedEvent)
		::CloseHandle(_onGpuCompletedEvent);
}

bool Fence_DX12::onCheckCompleted() {
	return _isCompleted(_lastSignaled);
}

void Fence_DX12::cpuWait(::UINT64 expectGpuCompletedValue) {
	if (_isCompleted(expectGpuCompletedValue)) {
		// lock-free checking
		return;
	}

	auto scopedLock = ScopedLock_make(_fenceWaitCS);
	// Double-check after acquiring the lock (avoid race condition between threads)
	if (_isCompleted(expectGpuCompletedValue)) {
		// locked checking
		return;
	}

	::HRESULT hr;
	hr = _d3dFence->SetEventOnCompletion(expectGpuCompletedValue, _onGpuCompletedEvent);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	::DWORD result = ::WaitForSingleObject(_onGpuCompletedEvent, INFINITE); // CPU thread is blocking here until the GPU signals the fence. (INFINITE is Wait Forever)
	if (result == WAIT_OBJECT_0) {
		// finished
	}

	_lastCompletedValue = _d3dFence->GetCompletedValue();
}

void Fence_DX12::cpuWait() {
	cpuWait(_lastSignaled);
}

u64 Fence_DX12::signal(::UINT64 fenceValue) {
	_lastSignaled		= fenceValue;
	_lastCompletedValue = fenceValue;
	_curSignal++;
	return _lastSignaled;
}

void Fence_DX12::_gpuWait(::ID3D12CommandQueue* d3dCmdQueue) {
	::HRESULT hr = d3dCmdQueue->Wait(_d3dFence, _curSignal);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);
}

::UINT64 Fence_DX12::_gpuSignal(::ID3D12CommandQueue* d3dCmdQueue) {
	::HRESULT hr = d3dCmdQueue->Signal(_d3dFence, _curSignal);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_lastSignaled = _curSignal;
	_curSignal++;
	return _lastSignaled;
}

bool Fence_DX12::_isCompleted(::UINT64 expectGpuCompletedValue) {
	if (!_d3dFence)
		AXE_THROW();

	if (expectGpuCompletedValue <= _lastCompletedValue) {
		return true;
	}

	_lastCompletedValue = Math::max(_lastCompletedValue, _d3dFence->GetCompletedValue());
	return expectGpuCompletedValue <= _lastCompletedValue; // true means gpu is done, false means cpu may keep waiting gpu.
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12