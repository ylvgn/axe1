#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderDevice.h>
#include "Common_DX12.h"

namespace axe {

class Device_DX12 : public RenderDevice {
	AXE_RTTI_INFO(Device_DX12, RenderDevice)
	using Util = DX12Util;
public:
	Device_DX12(CreateDesc& desc);

	DX12_IDXGIDevice*	dxgiDevice()	{ return _dxgiDevice; }
	DX12_ID3D12Device*	d3dDevice()		{ return _d3dDevice; }

private:
	static void s_D3D12MessageFunc(::D3D12_MESSAGE_CATEGORY Category
		, ::D3D12_MESSAGE_SEVERITY Severity
		, ::D3D12_MESSAGE_ID ID
		, ::LPCSTR pDescription
		, void* pContext)
	{
		auto* p = static_cast<Device_DX12*>(pContext);
		p->onD3D12MessageCallback(Category, Severity, ID, StrView(pDescription));
	}

	void onD3D12MessageCallback(::D3D12_MESSAGE_CATEGORY category
							  , ::D3D12_MESSAGE_SEVERITY severity
							  , ::D3D12_MESSAGE_ID id
							  , StrView description);

	ComPtr<DX12_IDXGIAdapter> _dxgiAdapter;
	ComPtr<DX12_IDXGIDevice>  _dxgiDevice;

	ComPtr<DX12_ID3D12Device> _d3dDevice;
}; // Device_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12