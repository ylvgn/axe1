#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/synchronized/RenderFence.h>
#include "Common_DX12.h"

namespace axe {

#if 0
#pragma mark ========= Fence_DX12 ============
#endif
/*
Fence Value Design:
	0  : init
	1  : signaled
	>1 : cpu is waiting
*/
class Fence_DX12 : public RenderFence {
	AXE_CLASS_TYPE(Fence_DX12, RenderFence)
	using Util = DX12Util;
public:
	Fence_DX12() noexcept;
	Fence_DX12(Device_DX12* device) noexcept;

	~Fence_DX12() { destroy(); }

	virtual bool onCheckCompleted() final;

	void create(const SrcLoc& srcLoc);
	void destroy();

	void resetFenceValue();
	void cpuWait();

	void gpuSignal(::ID3D12CommandQueue* d3dCmdQueue);
	void gpuWait(::ID3D12CommandQueue* d3dCmdQueue);

	DX12_ID3D12Fence* d3dFence() { return _d3dFence; }

private:
	bool _checkCompleted();
	bool _isCpuWaiting();

	ComPtr<DX12_ID3D12Fence> _d3dFence;
	::HANDLE				 _onGpuCompletedEvent;
}; // Fence_DX12


#if 0
#pragma mark ========= FencePool_DX12 ============
#endif
class FencePool_DX12 : public NonCopyable {
	using Util = DX12Util;
public:
	using Fence = Fence_DX12;

	FencePool_DX12();

    ~FencePool_DX12() { destroy(); }

    void create(Device_DX12* device);
	void destroy();

    Fence*	acquire();
	void	release(Fence* fence);

	void cpuWaitAll();

private:
	UPtr<Fence> _ctorNewFence();
	void		_pushBackToIdle();
	void		_popBackToRunning();

    Device_DX12*   _device = nullptr;
	Vector< UPtr<Fence>, 4> _idleList;
	Vector< UPtr<Fence>, 4> _runningList;
    Mutex		   _mutex;
}; // FencePool_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12