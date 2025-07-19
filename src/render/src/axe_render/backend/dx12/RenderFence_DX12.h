#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderFence.h>
#include "Render_DX12_Common.h"

namespace axe {

class RenderFence_DX12 : public RenderFence {
	using This = RenderFence_DX12;
	using Base = RenderFence;
	using Util = DX12Util;
public:
	RenderFence_DX12(CreateDesc& desc);
	~RenderFence_DX12();

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
}; // RenderFence_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12