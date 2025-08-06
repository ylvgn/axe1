#if AXE_RENDER_HAS_DX12

#include "Context_DX12.h"
#include "Renderer_DX12.h"
#include "Device_DX12.h"
#include <axe_render/vertex/Vertex.h>
#include <axe_render/command/RenderCommand.h>

namespace axe {
#if 0 // no need atm
LRESULT WINAPI Context_DX12::s_wndProc(::HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE: {
			auto cs = reinterpret_cast<::CREATESTRUCT*>(lParam);
			auto* thisObj = static_cast<This*>(cs->lpCreateParams);
			thisObj->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<::LONG_PTR>(thisObj));
		}break;

		case WM_DESTROY: {
			if (auto* thisObj = s_getThis(hwnd))
			{
				SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<::LONG_PTR>(nullptr));
				thisObj->_hwnd = nullptr;
			}
		}break;

		case WM_PAINT: {
			::PAINTSTRUCT ps;
			::BeginPaint(hwnd, &ps);
			if (auto* thisObj = s_getThis(hwnd)) {
				if (thisObj->_eventHandler) {
					thisObj->_eventHandler->render(thisObj);
				}
			}
			::EndPaint(hwnd, &ps);
			return 0;
		}break;

		case WM_SIZE: {
			if (auto* thisObj = s_getThis(hwnd))
			{
				auto* win = static_cast<NativeUIWindow_Win32*>(thisObj->window());
				win->setWorldRect(NativeUIWindow_Win32::s_win32_getWorldRect(hwnd));
				//AXE_DUMP_VAR(win->clientRect());
				return 0;
			}
		} break;

		case WM_SIZING: {
			if (auto* thisObj = s_getThis(hwnd))
			{
				auto* win = static_cast<NativeUIWindow_Win32*>(thisObj->window());
				win->setWorldRect(NativeUIWindow_Win32::s_win32_getWorldRect(hwnd));
			}
		} break;

		default: {
			if (auto* thisObj = s_getThis(hwnd)) {
				return thisObj->_window->_handleNativeEvent(hwnd, msg, wParam, lParam);
			}
		}break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}
#endif

Context_DX12::Context_DX12(RenderDevice* device, CreateDesc& desc)
	: Base(device, desc)
{
	AXE_ASSERT(desc.window->_hwnd);

//	_createWindow(desc); no use atm

	::HRESULT hr;

	auto* d3dDevice	  = _d3dDevice();
	auto* renderer	  = Util::renderer();
	auto* dxgiFactory = renderer->dxgiFactory();

	auto& _hwnd = desc.window->_hwnd;

	{ // create command queue
		::D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags	   = D3D12_COMMAND_QUEUE_FLAG_NONE; // D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
		queueDesc.NodeMask = 0;

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		hr			   = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_graphicsCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		hr			   = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_computeCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
#if 0
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		hr			   = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_copyCmdQueue.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
#endif
	}

	AXE_ASSERT(_hwnd);

	{ // create swap chain
		::DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount			  = kFrameBufferCount;
		swapChainDesc.Width					  = static_cast<int>(desc.window->clientRect().w); // 8; 
		swapChainDesc.Height				  = static_cast<int>(desc.window->clientRect().h); // 8; 
		swapChainDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage			  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect			  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count		  = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		dxgiFactory->CreateSwapChainForHwnd(_graphicsCmdQueue.ptr()
										  , _hwnd
										  , &swapChainDesc
										  , nullptr
										  , nullptr
										  , swapChain.ptrForInit());
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

		hr = swapChain->QueryInterface(IID_PPV_ARGS(m_swapChain.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
	}

	// This sample does not support fullscreen transitions.
	hr = dxgiFactory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		::D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors				 = kFrameBufferCount;
		rtvHeapDesc.Type						 = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags						 = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

		m_rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	_createRenderTargetView();
	
	// Create command allocator and list
	hr = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	// Create command list
	hr = d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.ptr(), nullptr, IID_PPV_ARGS(m_commandList.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	hr = m_commandList->Close();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
}

DX12_ID3D12Device* Context_DX12::_d3dDevice() {
	return renderDevice()->d3dDevice();
}

Device_DX12* Context_DX12::renderDevice() {
	return static_cast<Device_DX12*>(_device);
}

void Context_DX12::_createRenderTargetView() {
	_releaseRenderTargetView();

	::HRESULT hr;
	auto* d3dDevice = _d3dDevice();

	// Create frame resources.
	::CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart()); // using d3d12x.h

	// Create a RTV for each frame.
	for (UINT i = 0; i < kFrameBufferCount; i++) {
		hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTargets[i].ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

		d3dDevice->CreateRenderTargetView(m_renderTargets[i].ptr(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescriptorSize);
	}
}

void Context_DX12::_releaseRenderTargetView() {
	for (UINT i = 0; i < kFrameBufferCount; i++)
	{
		m_renderTargets[i].reset(nullptr);
	}
}

#if 0 // no need atm
void Context_DX12::_createWindow(CreateDesc& desc)
{
	auto   hInstance  = ::GetModuleHandle(nullptr);
	::HWND parentHwnd = desc.window ? desc.window->_hwnd : nullptr;

	static const wchar_t* kClassName = L"Context_DX12";

	// register window class
	::WNDCLASSEX tmpWc		= {};
	bool		 registered = (0 != ::GetClassInfoEx(hInstance, kClassName, &tmpWc));
	if (!registered)
	{
		tmpWc.cbSize		= sizeof(tmpWc);
		tmpWc.style			= CS_HREDRAW | CS_VREDRAW;
		tmpWc.lpfnWndProc	= &s_wndProc;
		tmpWc.cbClsExtra	= 0;
		tmpWc.cbWndExtra	= 0;
		tmpWc.hInstance		= hInstance;
		tmpWc.hIcon			= nullptr;
		tmpWc.hCursor		= LoadCursor(nullptr, IDC_ARROW);
		tmpWc.hbrBackground = nullptr; //(HBRUSH)(COLOR_WINDOW+1);
		tmpWc.lpszMenuName	= nullptr;
		tmpWc.lpszClassName = kClassName;
		tmpWc.hIconSm		= nullptr;

		if (!::RegisterClassEx(&tmpWc))
		{
			AXE_THROW();
		}
	}

	//--------
	::DWORD dwStyle	  = parentHwnd ? WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS : WS_POPUP;
	::DWORD dwExStyle = 0;
	_hwnd			  = ::CreateWindowEx(dwExStyle, kClassName, kClassName, dwStyle,
										 0, 0,
										 //static_cast<int>(desc.window->clientRect().w), static_cast<int>(desc.window->clientRect().h),
										 0, 0,
										 parentHwnd, nullptr, hInstance, this);
	if (!_hwnd)
	{
		AXE_THROW();
	}

	::ShowWindow(_hwnd, SW_SHOW);
}
#endif

void Context_DX12::_test_LoadAssets()
{
	using VertexT = VertexT_Color<Color4f, 1, Vertex_Pos>;

	::HRESULT hr;
	auto* d3dDevice = _d3dDevice();

	
    { // Create an empty root signature.
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
		hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, signature.ptrForInit(), error.ptrForInit());
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
		if (error) {
			OutputDebugStringA((char*)error->GetBufferPointer());
		}

        hr = d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
    }

    { // Create the pipeline state, which includes compiling and loading shaders.
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

		{ // Compile vertex shaders
			ComPtr<ID3DBlob> errorBlob;
			hr = D3DCompileFromFile(L"Assets/Shaders/Demo/hello_triangle.hlsl", nullptr, nullptr, "vs_main", "vs_5_0", compileFlags, 0, vertexShader.ptrForInit(), errorBlob.ptrForInit());
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
			if (errorBlob) {
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
		}

		{ // Compile pixel shaders
			ComPtr<ID3DBlob> errorBlob;
			hr = D3DCompileFromFile(L"Assets/Shaders/Demo/hello_triangle.hlsl", nullptr, nullptr, "ps_main", "ps_5_0", compileFlags, 0, pixelShader.ptrForInit(), nullptr);
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
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

        hr = d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
    }

	{ // Create the vertex buffer.
		float m_aspectRatio = swapchainFrameBufferSize().x / swapchainFrameBufferSize().y;

		VertexT triangleVertices[3];
		triangleVertices[0].pos.set( 0.00f,  0.25f * m_aspectRatio, 0.0f);	triangleVertices[0].color[0].set( 1.0f, 0.0f, 0.0f, 1.0f);
		triangleVertices[1].pos.set( 0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[1].color[0].set( 0.0f, 1.0f, 0.0f, 1.0f);
		triangleVertices[2].pos.set(-0.25f, -0.25f * m_aspectRatio, 0.0f);	triangleVertices[2].color[0].set( 0.0f, 0.0f, 1.0f, 1.0f);

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not
		// recommended. Every time the GPU needs it, the upload heap will be marshalled
		// over. Please read up on Default Heap usage. An upload heap is used here for
		// code simplicity and because there are very few verts to actually transfer.
		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		auto					resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		hr = d3dDevice->CreateCommittedResource(&heapProps,
												D3D12_HEAP_FLAG_NONE,
												&resDesc,
												D3D12_RESOURCE_STATE_GENERIC_READ,
												nullptr,
												IID_PPV_ARGS(m_vertexBuffer.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

		// Copy vectex data to the vertex buffer.
		UINT8*		  pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.

		hr = m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
		memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
		m_vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes  = sizeof(VertexT);
		m_vertexBufferView.SizeInBytes	  = vertexBufferSize;
	}

	// Create Fence (synchronization objects and wait until assets have been uploaded to the GPU)
	{
		hr = d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
		m_fenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
		}

		// Wait for the command list to execute; we are reusing the same command
		// list in our main loop but for now, we just want to wait for setup to
		// complete before continuing.
		_test_WaitForPreviousFrame();
	}
}

void Context_DX12::onBeginRender() {
	AXE_RUN_ONCE(_test_LoadAssets());

	::HRESULT hr;
	auto*	d3dDevice = _d3dDevice();

	// Record all the commands we need to render the scene into the command list.
	_test_PopulateCommandList();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.ptr() };
	_graphicsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	hr = m_swapChain->Present(1, 0);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	_test_WaitForPreviousFrame();
}

void Context_DX12::_test_PopulateCommandList() {
	::HRESULT hr;
	auto*	  d3dDevice = _d3dDevice();

	//  when you're trying to render to a swap chain back buffer that isn't the current back buffer
	/*
		D3D12 Validation Layer: ID3D12CommandQueue::ExecuteCommandLists: A command list, which writes to a swapchain back buffer,
		may only be executed when that back buffer is the back buffer that will be presented during the next call to Present*. 
		Such a back buffer is also referred to as the "current back buffer". 
		Swap Chain: 0x0000015DEBF5F4C0:'Unnamed Object' - Current Back Buffer Buffer: 0x0000015DE49EC4D0:'Unnamed ID3D12Resource Object' - Attempted Write Buffer: 0x0000015DE49EB4B0:'Unnamed ID3D12Resource Object* 
	*/
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex(); // make sure write to swap chain back buffer index

	// Command list allocators can only be reset when the associated
	// command lists have finished execution on the GPU; apps should use
	// fences to determine GPU execution progress.
	hr = m_commandAllocator->Reset();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	// However, when ExecuteCommandList() is called on a particular command
	// list, that command list can then be reset at any time and must be before
	// re-recording.
	hr = m_commandList->Reset(m_commandAllocator.ptr(), m_pipelineState.ptr());
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	// Set necessary state.
	m_commandList->SetGraphicsRootSignature(m_rootSignature.ptr());

	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;
	m_viewport.Width	= swapchainFrameBufferSize().x;
	m_viewport.Height	= swapchainFrameBufferSize().y;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_commandList->RSSetViewports(1, &m_viewport);

	m_scissorRect.left	 = 0;
	m_scissorRect.top	 = 0;
	m_scissorRect.right	 = static_cast<::LONG>(swapchainFrameBufferSize().x);
	m_scissorRect.bottom = static_cast<::LONG>(swapchainFrameBufferSize().y);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate that the back buffer will be used as a render target. (Transition back buffer to render target)
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].ptr(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier);

	::CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);

	// Indicate that the back buffer will now be used to present. (Transition back buffer to present)
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].ptr(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrier);

	hr = m_commandList->Close();
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
}

void Context_DX12::_test_WaitForPreviousFrame() {
	::HRESULT hr;
	auto*	  d3dDevice = _d3dDevice();

	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. More advanced samples
	// illustrate how to use fences for efficient resource usage.

	// Signal and increment the fence value.
	const UINT64 curFenceValue = m_fenceValue;
	hr = _graphicsCmdQueue->Signal(m_fence.ptr(), curFenceValue);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	m_fenceValue++;

	// Wait until the previous frame is finished.
	if (m_fence->GetCompletedValue() < curFenceValue)
	{
		hr = m_fence->SetEventOnCompletion(curFenceValue, m_fenceEvent);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}
#if 0 // no need atm
void Context_DX12::onSetNeedToRender() {
	if (_hwnd) {
		::RedrawWindow(_hwnd, nullptr, nullptr, RDW_INVALIDATE);
	}
}
#endif

void Context_DX12::onSetSwapchainFrameBufferSize(const Vec2f& newSize) {
	::HRESULT hr;
	auto*	  d3dDevice = _d3dDevice();

	_releaseRenderTargetView();

	auto width	= static_cast<UINT>(Math::max(8.0f, newSize.x));
	auto height = static_cast<UINT>(Math::max(8.0f, newSize.y));
	hr	= m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice);

	_createRenderTargetView();

	Base::onSetSwapchainFrameBufferSize(newSize);
}

void Context_DX12::onCommit(RenderCommandBuffer& cmdBuf)
{
	_dispatch(this, cmdBuf);
}

void Context_DX12::onCmd_ClearFrameBuffers(RenderCommand_ClearFrameBuffers& cmd) {
	#if 0 // TODO
	auto* ctx = _renderer->d3dDeviceContext();

	// clear back buffer(color buffer)
	if (_renderTargetView && cmd.color.has_value()) {
		ctx->ClearRenderTargetView(_renderTargetView, cmd.color->data);
	}

	// clear depth&stencil buffer
	if (_depthStencilView && (cmd.depth.has_value() || cmd.stencil.has_value())) {
		ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, *cmd.depth, static_cast<UINT8>(*cmd.stencil));
	}
	#endif
}

void Context_DX12::onEndRender() {
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12