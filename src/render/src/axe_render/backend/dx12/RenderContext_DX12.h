#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderContext.h>
#include "Render_DX12_Common.h"

namespace axe {

class RenderContext_DX12 : public RenderContext {
	using This = RenderContext_DX12;
	using Base = RenderContext;
	using Util = DX12Util;
public:
	RenderContext_DX12(CreateDesc& desc);

	static const UINT FrameCount = 2;

	// Pipeline objects.
	D3D12_VIEWPORT				m_viewport;
	ComPtr<DX12_IDXGISwapChain> m_swapChain; // TODO RenderSwapChain

	D3D12_RECT m_scissorRect;

	ComPtr<ID3D12Resource>			  m_renderTargets[FrameCount];
	ComPtr<ID3D12CommandAllocator>	  m_commandAllocator;
	ComPtr<ID3D12CommandQueue>		  m_commandQueue;
	ComPtr<ID3D12RootSignature>		  m_rootSignature;
	ComPtr<ID3D12DescriptorHeap>	  m_rtvHeap;
	ComPtr<ID3D12PipelineState>		  m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT							  m_rtvDescriptorSize;

	// App resources.
	ComPtr<ID3D12Resource>	 m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	// Synchronization objects.
	UINT   m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence; // TODO RenderFence
	UINT64 m_fenceValue;

	virtual void onBeginRender() override;
	virtual void onEndRender() override;

private:
	void _test_LoadAssets();
	void _test_PopulateCommandList();
	void _test_WaitForPreviousFrame();

	AXE_INLINE DX12_ID3D12Device* _d3dDevice();

	void _createRenderTargetView();
	void _releaseRenderTargetView();

	RenderDevice_DX12* _device = nullptr;

}; // RenderContext_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12