#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/synchronized/RenderFence.h>
#include "Common_DX12.h"

namespace axe {

#if 0
#pragma mark ========= Fence_DX12 ============
#endif

class Fence_DX12 : public RenderFence {
	AXE_RTTI_INFO(Fence_DX12, RenderFence)
	using Util = DX12Util;
public:
	using Base::Base;

	~Fence_DX12() { destroy(); }

	virtual bool onCheckCompleted() override;

	void create(Device_DX12* device);
	void destroy();

	DX12_ID3D12Fence* d3dFence() { return _d3dFence; }
protected:
	ComPtr<DX12_ID3D12Fence> _d3dFence;
	::HANDLE				 _onGpuCompletedEvent;
}; // Fence_DX12


#if 0
#pragma mark ========= FencePool_DX12 ============
#endif
class FencePool_DX12 : public NonCopyable {
	using Util = DX12Util;
public:
	class Fence : public Fence_DX12 {
		AXE_RTTI_INFO(Fence, Fence_DX12)
		using Util = DX12Util;
	friend class FencePool_DX12;
	public:
		void create(const SrcLoc& srcLoc);

		virtual bool onCheckCompleted() final;

		void cpuWait();

		void gpuSignal(::ID3D12CommandQueue* d3dCmdQueue);
		void gpuWait(::ID3D12CommandQueue* d3dCmdQueue);
	protected:
		Fence::Fence(Device_DX12& device) noexcept;
	private:
		Fence() = delete;
		void resetFenceValue();
		bool _isCpuWaiting();
	}; // Fence

	FencePool_DX12();

    ~FencePool_DX12() { destroy(); }

    void create(Device_DX12* device);
	void destroy();

    Fence*	acquire(const SrcLoc& srcLoc);
	void	release(Fence* fence);

	void cpuWaitAll();

private:
	void		_pushBackToIdle();
	void		_popBackToRunning();
	UPtr<Fence> _ctorNewFence(const SrcLoc& srcLoc);

    Device_DX12*   _device = nullptr;
	Vector< UPtr<Fence>, 4> _idleList;
	Vector< UPtr<Fence>, 4> _runningList;
    Mutex		   _mutex;
}; // FencePool_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12