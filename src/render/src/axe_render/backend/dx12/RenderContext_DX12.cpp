#if AXE_RENDER_HAS_DX12

#include "RenderContext_DX12.h"
#include "Device_DX12.h"
#include "Renderer_DX12.h"

#include <axe_render/vertex/Vertex.h>
#include <axe_render/command/RenderCommand.h>
#include "SwapChain_DX12.h"

namespace axe {

DX12_ID3D12Device* RenderContext_DX12::d3dDevice() {
	return renderDevice()->d3dDevice();
}

Device_DX12* RenderContext_DX12::renderDevice() {
	return static_cast<Device_DX12*>(_device);
}

RenderContext_DX12::RenderContext_DX12(RenderDevice* device, const CreateDesc& desc)
	: Base(device, desc)
{
	::HRESULT hr;

	auto* d3d12Device = d3dDevice();

	{ // create command queue
		::D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags	   = D3D12_COMMAND_QUEUE_FLAG_NONE; // D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
		queueDesc.NodeMask = 0;

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hr			   = d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_graphicsCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr			   = d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_computeCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

#if 0 // no use atm
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		hr			   = d3d12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_copyCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
#endif
	}

	_swapChain.reset(new SwapChain_DX12());
	_swapChain->create(this);

	{ // disable alt+enter
		auto* renderer	   = Util::renderer();
		auto* dxgiFactory  = renderer->dxgiFactory();
		auto& _hwnd = desc.window->_hwnd;
		hr = dxgiFactory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
	}

	// Create command allocator and list
	hr = d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

	// Create command list
	hr = d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.ptr(), nullptr, IID_PPV_ARGS(m_commandList.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	hr = m_commandList->Close();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
}

void RenderContext_DX12::_test_LoadAssets() {
	using VertexT = VertexT_Color<Color4f, 1, Vertex_Pos>;

	::HRESULT hr;
	auto* d3d12Device = d3dDevice();

    { // Create an empty root signature.
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.ptrForInit(), error.ptrForInit());
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
		if (error) {
			OutputDebugStringA((char*)error->GetBufferPointer());
		}

        hr = d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
    }

    { // Create the pipeline state, which includes compiling and loading shaders.
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if AXE_RENDER_DEBUG_LAYER
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

		{ // Compile vertex shaders
			ComPtr<ID3DBlob> errorBlob;
			hr = D3DCompileFromFile(L"Assets/Shaders/Demo/hello_triangle.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", compileFlags, 0, vertexShader.ptrForInit(), errorBlob.ptrForInit());
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
		}

		{ // Compile pixel shaders
			ComPtr<ID3DBlob> errorBlob;
			hr = D3DCompileFromFile(L"Assets/Shaders/Demo/hello_triangle.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", compileFlags, 0, pixelShader.ptrForInit(), nullptr);
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
		}

        // Define the vertex input layout.
        ::D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }, // float3
            { "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }  // float4
        };

        // create the graphics pipeline state object (PSO).
        ::D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc= {};
        psoDesc.InputLayout							= { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature						= m_rootSignature.ptr();
        psoDesc.VS									= { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
        psoDesc.PS									= { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState						= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState							= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable		= FALSE;
        psoDesc.DepthStencilState.StencilEnable		= FALSE;
        psoDesc.SampleMask							= UINT_MAX;
        psoDesc.PrimitiveTopologyType				= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets					= 1;
        psoDesc.RTVFormats[0]						= DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count					= 1;

        hr = d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
    }

	{ // Create the vertex buffer.
		float m_aspectRatio = swapChainFrameBufferSize().x / swapChainFrameBufferSize().y;

		VertexT triangleVertices[3];
		triangleVertices[0].pos.set( 0.00f,  0.25f * m_aspectRatio, 0.0f);	triangleVertices[0].color[0].set( 1.0f, 0.0f, 0.0f, 1.0f);
		triangleVertices[1].pos.set( 0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[1].color[0].set( 0.0f, 1.0f, 0.0f, 1.0f);
		triangleVertices[2].pos.set(-0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[2].color[0].set( 0.0f, 0.0f, 1.0f, 1.0f);

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not
		// recommended. Every time the GPU needs it, the upload heap will be marshalled
		// over. Please read up on Default Heap usage. An upload heap is used here for
		// code simplicity and because there are very few verts to actually transfer.
		::CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		auto					  resDesc = ::CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		hr = d3d12Device->CreateCommittedResource(&heapProps,
												D3D12_HEAP_FLAG_NONE,
												&resDesc,
												D3D12_RESOURCE_STATE_GENERIC_READ,
												nullptr,
												IID_PPV_ARGS(m_vertexBuffer.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		// Copy vectex data to the vertex buffer.
		UINT8*		  pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.

		hr = m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
		memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes  = sizeof(VertexT);
		m_vertexBufferView.SizeInBytes	  = vertexBufferSize;
	}

	// Create Fence (synchronization objects and wait until assets have been uploaded to the GPU)
	{
		hr = d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
		m_fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
		}

		// Wait for the command list to execute; 
		// we are reusing the same command list in our main loop
		// but for now, we just want to wait for setup to complete before continuing.
		_test_WaitForPreviousFrame();
	}
}

void RenderContext_DX12::onBeginRender() {
	AXE_RUN_ONCE(_test_LoadAssets());

	::HRESULT hr;
	auto*	  d3d12Device = d3dDevice();

	// Command list allocators can only be reset when the associated
	// command lists have finished execution on the GPU; apps should use
	// fences to determine GPU execution progress.
	hr = m_commandAllocator->Reset();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

	// However, when ExecuteCommandList() is called on a particular command
	// list, that command list can then be reset at any time and must be before
	// re-recording.
	hr = m_commandList->Reset(m_commandAllocator.ptr(), m_pipelineState.ptr());
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

	// Set necessary state.
	m_commandList->SetGraphicsRootSignature(m_rootSignature.ptr());

	// Indicate that the back buffer will be used as a render target. (Transition back buffer to render target)
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(_swapChain->d3dRTV()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier);

	auto rtvHandle = _swapChain->d3dRTVHandle().cpu;
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void RenderContext_DX12::_test_WaitForPreviousFrame() {
	::HRESULT hr;
	auto* d3d12Device = d3dDevice();

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity.
	// More advanced samples illustrate how to use fences for efficient resource usage.

	// Signal and increment the fence value.
	const UINT64 curFenceValue = m_fenceValue;
	hr = _graphicsCmdQueue->Signal(m_fence.ptr(), curFenceValue);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < curFenceValue)
	{
		hr = m_fence->SetEventOnCompletion(curFenceValue, m_fenceEvent);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void RenderContext_DX12::onSetSwapChainFrameBufferSize(const Vec2f& newSize) {
	Base::onSetSwapChainFrameBufferSize(newSize);
	_swapChain->OnResizeOrMove(newSize);
}

void RenderContext_DX12::onCommit(RenderCommandBuffer& cmdBuf) {
	_dispatch(this, cmdBuf);
}

void RenderContext_DX12::onCmd_SetViewport(RenderCommand_SetViewport& cmd) {
	auto& rect = cmd.rect;

	::D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = rect.x;
	viewport.TopLeftY = rect.y;
	viewport.Width	  = rect.w;
	viewport.Height	  = rect.h;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	m_commandList->RSSetViewports(1, &viewport);
}

void RenderContext_DX12::onCmd_SetScissorRect(RenderCommand_SetScissorRect& cmd) {
	auto& rect = cmd.rect;

	::D3D12_RECT scissorRect = {};
	using DST = decltype(scissorRect.left);

	scissorRect.left    = static_cast<DST>(rect.x);
	scissorRect.top		= static_cast<DST>(rect.y);
	scissorRect.right	= static_cast<DST>(rect.xMax());
	scissorRect.bottom	= static_cast<DST>(rect.yMax());
	m_commandList->RSSetScissorRects(1, &scissorRect);
}

void RenderContext_DX12::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	// clear back buffer(color buffer)
	if (cmd.color.has_value()) {
		auto rtvHandle = _swapChain->d3dRTVHandle();
		m_commandList->ClearRenderTargetView(rtvHandle, cmd.color.value().data, 0, nullptr);
	}

#if 0 // TODO
	// clear depth&stencil buffer
	if (_depthStencilView && (cmd.depth.has_value() || cmd.stencil.has_value())) {
		ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, static_cast<UINT8>(*cmd.stencil));
	}
#endif
}

void RenderContext_DX12::onCmd_SwapBuffers(RenderCommand_SwapBuffers& cmd) {
	::HRESULT hr;
	auto*	  d3d12Device = d3dDevice();

	// Indicate that the back buffer will now be used to present. (Transition back buffer to present)
	auto barrier = ::CD3DX12_RESOURCE_BARRIER::Transition(_swapChain->d3dRTV()
		, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrier);

	hr = m_commandList->Close();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
	
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.ptr() };
	_graphicsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

#if 0 // TODO
	{
		auto* fence = ax_type_cast_debug<axDX12Fence>(dispatcher.renderRequest->completedFence());
		fence->addToGpu(dispatcher.cmdQueue);
	}
#endif

	// Present the frame.
	_swapChain->present();
}

void RenderContext_DX12::onCmd_DrawCall(RenderCommand_DrawCall& cmd) {
	m_commandList->IASetPrimitiveTopology(DX12Util::getDxPrimitiveTopology(cmd.primitive));

	AXE_TODO("draw vertex buffer");
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);
}

void RenderContext_DX12::onEndRender() {
	_test_WaitForPreviousFrame();
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12