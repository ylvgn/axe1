#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/synchronized/RenderFence.h>
#include "Common_DX12.h"

namespace axe {

class Fence_DX12 : public RenderFence {
	using This = Fence_DX12;
	using Base = RenderFence;
	using Util = DX12Util;
public:
	Fence_DX12(CreateDesc& desc);
	~Fence_DX12();

	DX12_ID3D12Fence* d3dFence() { return _d3dFence; }

	virtual bool onCheckCompleted() final;

	void cpuWait(UINT64 expectGpuCompletedValue);
	void cpuWait();

private:
	void	_gpuWait(::ID3D12CommandQueue* d3dCmdQueue);
	UINT64	_gpuSignal(::ID3D12CommandQueue* d3dCmdQueue);

	ComPtr<DX12_ID3D12Fence> _d3dFence;

	UINT64 _curSignal;
	UINT64 _lastSignaled;

	UINT64 _lastCompletedValue;

	HANDLE _onGpuCompletedEvent;

	Mutex _fenceWaitCS;
}; // Fence_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12