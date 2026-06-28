#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderContext.h>
#include "Common_DX12.h"

namespace axe {

class RenderContext_DX12 : public RenderContext {
	AXE_RTTI_INFO(RenderContext_DX12, RenderContext)
	using Util = DX12Util;
public:
	RenderContext_DX12(RenderDevice* device, const CreateDesc& desc);

	ComPtr<ID3D12RootSignature>		  m_rootSignature;
	ComPtr<ID3D12PipelineState>		  m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator>	  m_commandAllocator;

	// App resources.
	ComPtr<ID3D12Resource>			  m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		  m_vertexBufferView;

	// Synchronization objects.
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence; // TODO Fence_DX12
	UINT64 m_fenceValue;

	virtual void onBeginRender() final;
	virtual void onEndRender() final;

	Device_DX12*				renderDevice();
	DX12_ID3D12Device*			d3dDevice();
	DX12_ID3D12CommandQueue*	d3dGraphicsCmdQueue() { return _graphicsCmdQueue; }

	virtual void onSetSwapChainFrameBufferSize(const Vec2f& newSize) final;
	virtual void onCommit(RenderCommandBuffer& cmdBuf) final;

	void onCmd_SetViewport			(RenderCommand_SetViewport& cmd);
	void onCmd_SetScissorRect		(RenderCommand_SetScissorRect& cmd);
	void onCmd_ClearFrameBuffers	(RenderCommand_ClearFrameBuffers& cmd);
	void onCmd_SwapBuffers			(RenderCommand_SwapBuffers& cmd);
	void onCmd_DrawCall				(RenderCommand_DrawCall& cmd);

private:
	void _test_LoadAssets();
	void _test_WaitForPreviousFrame();

	UPtr<SwapChain_DX12>			_swapChain;
	ComPtr<DX12_ID3D12CommandQueue> _graphicsCmdQueue;
	ComPtr<DX12_ID3D12CommandQueue> _computeCmdQueue;
//	ComPtr<DX12_ID3D12CommandQueue> _copyCmdQueue; // no use atm

}; // RenderContext_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12