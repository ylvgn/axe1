#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/synchronized/RenderFence.h>
#include "Common_DX12.h"

namespace axe {

class Fence_DX12 : public RenderFence {
	AXE_CLASS_TYPE(Fence_DX12, RenderFence)
	using Util = DX12Util;
public:
	virtual void onCreate(CreateDesc& desc) final;

	~Fence_DX12();

	virtual bool onCheckCompleted() final;

	void cpuWait(::UINT64 expectGpuCompletedValue);
	void cpuWait();

	::UINT64 signal(::UINT64 fenceValue);

	DX12_ID3D12Fence* d3dFence() { return _d3dFence; }

private:
	void		_gpuWait(::ID3D12CommandQueue* d3dCmdQueue);
	::UINT64	_gpuSignal(::ID3D12CommandQueue* d3dCmdQueue);

	bool _isCompleted(::UINT64 expectGpuCompletedValue);

	ComPtr<DX12_ID3D12Fence> _d3dFence;

	::UINT64 _curSignal;
	::UINT64 _lastSignaled = 0;

	::UINT64 _lastCompletedValue;

	::HANDLE _onGpuCompletedEvent;

	Mutex	 _fenceWaitCS;
}; // Fence_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12