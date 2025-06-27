#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderDevice.h>
#include "Render_DX12_Common.h"

namespace axe {

class RenderDevice_DX12 : public RenderDevice {
	using This = RenderDevice_DX12;
	using Base = RenderDevice;
	using Util = DX12Util;
public:
	RenderDevice_DX12(CreateDesc& desc);

	DX12_IDXGIDevice*	dxgiDevice()	{ return _dxgiDevice; }
	DX12_ID3D12Device*	d3dDevice()		{ return _d3dDevice; }

protected:
	axeRenderDevice_InterfaceFunctions(DX12);

private:
	ComPtr<DX12_IDXGIAdapter> _dxgiAdapter;
	ComPtr<DX12_IDXGIDevice>  _dxgiDevice;

	ComPtr<DX12_ID3D12Device> _d3dDevice;

}; // RenderDevice_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12