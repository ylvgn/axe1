#if AXE_RENDER_HAS_DX12

#include "Fence_DX12.h"
#include "Device_DX12.h"

namespace axe  {

#if 0
#pragma mark ========= Fence_DX12 ============
#endif
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

void Fence_DX12::create(Device_DX12* device) {
	AXE_ASSERT(device);

	destroy();

	_device = device;
	_signaled = 0;

	auto* d3dDevice = device->d3dDevice();

	::HRESULT hr;
	hr = d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_d3dFence.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	_onGpuCompletedEvent = ::CreateEvent(nullptr, false, false, L"DX12 Fence Event");
	if (!_onGpuCompletedEvent)
		AXE_THROW;
}

void Fence_DX12::destroy() {
	if (_d3dFence && !onCheckCompleted()) {
		AXE_THROW;
	}
	if (_onGpuCompletedEvent) {
		::CloseHandle(_onGpuCompletedEvent);
		_onGpuCompletedEvent = nullptr;
	}
	if (_d3dFence) {
		_d3dFence.reset(nullptr);
	}
}

bool Fence_DX12::onCheckCompleted() {
	if (!_d3dFence)
		AXE_THROW;
	return _signaled <= _d3dFence->GetCompletedValue();
}


#if 0
#pragma mark ========= FencePool_DX12::Fence ============
#endif
FencePool_DX12::Fence::Fence(Device_DX12* device) noexcept
	: Base(device)
{
/*
	Fence Value Design:
		0  : init
		1  : signaled
		>1 : cpu is waiting
*/
}

void FencePool_DX12::Fence::create(const SrcLoc& srcLoc) {
	Base::create(static_cast<Device_DX12*>(_device));

#if defined(_DEBUG)
	auto debugName = TempString::s_format("{}", srcLoc);
	Util::setDebugName(_d3dFence.ptr(), debugName);
#endif
}

bool FencePool_DX12::Fence::_isCpuWaiting() {
	return _signaled > 1;
}

void FencePool_DX12::Fence::cpuWait() {
	if (onCheckCompleted()) {
		return;
	}
	if (_isCpuWaiting()) {
		return;
	}

	++ _signaled;

	::HRESULT hr;
	hr = _d3dFence->SetEventOnCompletion(1, _onGpuCompletedEvent);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	::DWORD result = ::WaitForSingleObject(_onGpuCompletedEvent, INFINITE); // CPU thread is blocking here until the GPU signals the fence. (INFINITE is Wait Forever)
	if (result == WAIT_OBJECT_0) {
		// cpu side finished
	}
	resetFenceValue();
}

void FencePool_DX12::Fence::resetFenceValue() {
	if (!onCheckCompleted()) {
		AXE_ASSERT(false);
	}
	_signaled = 0;
	_d3dFence->Signal(0);
}

void FencePool_DX12::Fence::gpuWait(::ID3D12CommandQueue* d3dCmdQueue) {
	if (_isCpuWaiting()) {
		return;
	}
	_signaled = 1;
	AXE_DX12_THROWIF_HRESULT_ERROR(d3dCmdQueue->Wait(_d3dFence, _signaled));
}

void FencePool_DX12::Fence::gpuSignal(::ID3D12CommandQueue* d3dCmdQueue) {
	if (_isCpuWaiting()) {
		return;
	}
	_signaled = 1;
	AXE_DX12_THROWIF_HRESULT_ERROR(d3dCmdQueue->Signal(_d3dFence, _signaled));
}

bool FencePool_DX12::Fence::onCheckCompleted() {
	if (!_d3dFence)
		AXE_THROW;
	return _signaled == 0 || _d3dFence->GetCompletedValue() > 0;
}


#if 0
#pragma mark ========= FencePool_DX12 ============
#endif
FencePool_DX12::FencePool_DX12() {
	create(Util::rootDevice());
}

void FencePool_DX12::create(Device_DX12* device) {
	destroy();

	_device = device;

	if (_idleList.empty()) {
		_idleList.resizeToLocalBufSize();
		for (int i = 0; i < _idleList.size(); ++i) {
			_idleList[i] = AXE_MOVE(_ctorNewFence(AXE_LOC));
		}
	}
}

void FencePool_DX12::destroy() {
	cpuWaitAll();
	for (auto i = _runningList.size(); i > 0; --i) {
		_pushBackToIdle();
	}
	_runningList.clear();
}

FencePool_DX12::Fence* FencePool_DX12::acquire(const SrcLoc& srcLoc) {
	auto scopedLock = _mutex.scopedLock();
    if (!_idleList.empty()) {
		_popBackToRunning();
    } else {
		_runningList.emplace_back(AXE_MOVE(_ctorNewFence(srcLoc)));
    }
	return _runningList.back().get();
}

void FencePool_DX12::release(Fence* fence) {
	auto scopedLock = _mutex.scopedLock();
	auto it	= ::eastl::find(_runningList.begin(), _runningList.end(), fence, [](auto& first, auto& target) {
		return first.get() == target;
	});
	if (it != _runningList.end()) {
		auto* tmp = _runningList.erase(it);
		_idleList.emplace_back(AXE_MOVE(*tmp));
	}
}

void FencePool_DX12::cpuWaitAll() {
	for (auto& fence : _runningList) {
		fence->cpuWait();
	}
}

void FencePool_DX12::_pushBackToIdle() {
	UPtr<Fence> tmp = AXE_MOVE(_runningList.back());
	_runningList.pop_back();
	_idleList.emplace_back(AXE_MOVE(tmp));
}

void FencePool_DX12::_popBackToRunning() {
	UPtr<Fence> tmp = AXE_MOVE(_idleList.back());
	_idleList.pop_back();
	tmp->resetFenceValue();
	_runningList.emplace_back(AXE_MOVE(tmp));
}

UPtr<FencePool_DX12::Fence> FencePool_DX12::_ctorNewFence(const SrcLoc& srcLoc){
	auto f = UPtr<Fence>(new Fence(_device));
	f->create(srcLoc);
	return f;
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12