#if AXE_RENDER_HAS_DX12

#include "SwapChain_DX12.h"
#include "Renderer_DX12.h"
#include "RenderContext_DX12.h"
#include "Capabilities_DX12.h"

namespace axe {

#if 0
#pragma mark ========= Texture2D_GL__Format ============
#endif
void SwapChain_DX12__Format::set(MonitorDisplayMode mode) {
	using SRC = MonitorDisplayMode;
	switch (mode) {
		case SRC::SDR:		{ colorSpaceFormat = DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;	 swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM; } break;
		case SRC::HDR_PQ:	{ colorSpaceFormat = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020; swapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM; } break;
		case SRC::HDR_scRGB:{ colorSpaceFormat = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;	 swapChainFormat = DXGI_FORMAT_R16G16B16A16_FLOAT; } break;
	//---
		default:
			AXE_THROW;
	}
}

#if 0
#pragma mark ========= SwapChain_DX12 ============
#endif

DescriptorHandle_DX12 SwapChain_DX12::d3dRTVHandle() {
	DescriptorHandle_DX12 h;
	h.cpu = DescriptorHandle_DX12::CpuHandle(
		_rtvHeap->GetCPUDescriptorHandleForHeapStart()
		, _curImageIdx
		, _rtvDescriptorSize
	);
	return h;
}

RenderContext_DX12* SwapChain_DX12::renderContext()
{
	return static_cast<RenderContext_DX12*>(_renderContext);
}

DX12_ID3D12Device* SwapChain_DX12::d3dDevice(){
	return renderContext()->d3dDevice();
}

void SwapChain_DX12::present() {
	auto* device	= _renderContext->device();
	bool  vsync		= device->vsync();
	bool hasTearing = device->capabilities()->hasTearing();

	::HRESULT hr;
	hr = _d3dSwapChain->Present(vsync ? 1 : 0, !vsync && hasTearing ? DXGI_PRESENT_ALLOW_TEARING : 0);
	AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3dDevice());

	_refreshCurImageIndex();

#if 0
	// Signal and store when the GPU work for the frame we just flipped is finished.
	m_pPresentFence->Signal(renderContext()->d3dGraphicsCmdQueue(););
	WaitForSingleObject(m_WaitableObject, INFINITE);
#endif
}

void SwapChain_DX12::destroy() {
	AXE_TODO("");
}

void SwapChain_DX12::OnResizeOrMove(const Vec2i& newSize) {
	if (/* desiredFormat != _format ||*/ _frameBufferSize != newSize) {
		_frameBufferSize = newSize;

		::HRESULT hr;
		auto* d3d12Device = d3dDevice();

		_releaseRenderTargetView(); // before call ResizeBuffers, must release old render target view.
		Vec2i frameSize = Math::max(kMinFrameSize, newSize);
		
		hr = _d3dSwapChain->ResizeBuffers(0
										, frameSize.x
										, frameSize.y
										, DXGI_FORMAT_UNKNOWN
										, 0);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		_createRenderTargetView();
	}
}

bool SwapChain_DX12::_isDisplaySupportsHDR() const {
	AXE_TODO("get this info from adapter. and maybe use HWND to get HMonitor then found out cur swap chain is using which monitor");
	return false;
}

void SwapChain_DX12::create(RenderContext_DX12* context) {
	AXE_ASSERT(context);

	destroy();

	::HRESULT hr;

	auto* renderer	  = Util::renderer();
	auto* dxgiFactory = renderer->dxgiFactory();
	auto* d3d12Device = context->d3dDevice();

	const auto& _hwnd = context->hwnd();

	AXE_ASSERT(_hwnd);

	_renderContext = context;

	{ // create swap chain
		::DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount			  = kFrameBufferCount;
		swapChainDesc.Width					  = kMinFrameSize.x;
		swapChainDesc.Height				  = kMinFrameSize.y;
		swapChainDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage			  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect			  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count		  = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		hr = dxgiFactory->CreateSwapChainForHwnd(context->d3dGraphicsCmdQueue()
											   , _hwnd
											   , &swapChainDesc
											   , nullptr
											   , nullptr
											   , swapChain.ptrForInit());
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		hr = swapChain->QueryInterface(IID_PPV_ARGS(_d3dSwapChain.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);
	}

	{ // Create descriptor heaps.
		// Describe and create a render target view (RTV) descriptor heap.
		::D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors				 = kFrameBufferCount;
		rtvHeapDesc.Type						 = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags						 = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		hr = d3d12Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(_rtvHeap.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		_rtvDescriptorSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	_createRenderTargetView();
}

void SwapChain_DX12::_releaseRenderTargetView() {
	for (::UINT i = 0; i < kFrameBufferCount; ++i) {
		_renderTargets[i].reset(nullptr);
	}
}

void SwapChain_DX12::_createRenderTargetView() {
	_releaseRenderTargetView();

	::HRESULT hr;
	auto* d3d12Device = d3dDevice();

	// Create frame resources.
	DescriptorHandle_DX12 rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each frame.
	for (::UINT i = 0; i < kFrameBufferCount; ++i) {
		hr = _d3dSwapChain->GetBuffer(i, IID_PPV_ARGS(_renderTargets[i].ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, d3d12Device);

		d3d12Device->CreateRenderTargetView(_renderTargets[i].ptr(), nullptr, rtvHandle);
		rtvHandle.cpu += _rtvDescriptorSize;
	}

	_refreshCurImageIndex();
}

void SwapChain_DX12::_refreshCurImageIndex() {
	_curImageIdx = _d3dSwapChain->GetCurrentBackBufferIndex();
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12