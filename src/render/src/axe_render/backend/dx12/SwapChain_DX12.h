#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/synchronized/RenderSwapChain.h>
#include "Common_DX12.h"
#include "DescripterHeap_DX12.h"

namespace axe {

#if 0
#pragma mark ========= SwapChain_DX12__Format ============
#endif
class SwapChain_DX12__Format {
public:
	explicit SwapChain_DX12__Format() = default;
	explicit SwapChain_DX12__Format(MonitorDisplayMode mode) noexcept { set(mode); }

	void set(MonitorDisplayMode mode);

	::DXGI_COLOR_SPACE_TYPE   colorSpaceFormat;
	::DXGI_FORMAT			  swapChainFormat;
}; // SwapChain_DX12__Format


#if 0
#pragma mark ========= SwapChain_DX12 ============
#endif
class SwapChain_DX12 : public RenderSwapChain {
	AXE_RTTI_INFO(SwapChain_DX12, RenderSwapChain)
	using Format = SwapChain_DX12__Format;
	using Util = DX12Util;
public:
	using Base::Base;

	~SwapChain_DX12() { destroy(); }

	void create(RenderContext_DX12* context);
	void destroy();

	virtual void OnResizeOrMove(const Vec2f& newSize) final;

	DX12_IDXGISwapChain*		d3dSwapChain()	{ return _d3dSwapChain; }
	DX12_ID3D12Resource*		d3dRTV()		{ return _renderTargets[_curImageIdx]; }
	DX12_ID3D12DescriptorHeap*	d3dHeap()		{ return _rtvHeap; }
	DescriptorHandle_DX12		d3dRTVHandle();
	DX12_ID3D12Device*			d3dDevice();

	RenderContext_DX12*		  renderContext();

	void present();

private:
	void _createRenderTargetView();
	void _releaseRenderTargetView();

	void _refreshCurImageIndex();

	bool _isDisplaySupportsHDR() const;

	Format						 _format; // TODO

	::HANDLE _waitableEvent = nullptr;

	ComPtr<DX12_IDXGISwapChain>			_d3dSwapChain;
	ComPtr<DX12_ID3D12Resource>			_renderTargets[kFrameBufferCount];
	ComPtr<DX12_ID3D12DescriptorHeap>	_rtvHeap;
	UINT								_rtvDescriptorSize;
}; // SwapChain_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12