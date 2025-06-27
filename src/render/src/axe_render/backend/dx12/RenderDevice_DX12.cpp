#if AXE_RENDER_HAS_DX12

#include "RenderDevice_DX12.h"
#include "RenderContext_DX12.h"
#include "Renderer_DX12.h"
#include "RenderCapabilities_DX12.h"

namespace axe {

axeRenderDevice_InterfaceFunctions_Impl(DX12)

RenderDevice_DX12::RenderDevice_DX12(CreateDesc& desc)
	: Base(desc)
{
	::HRESULT hr;

	auto* renderer	  = Renderer_DX12::s_instance();
	auto* dxgiFactory = renderer->dxgiFactory();

	if (desc.adapterInfo) {
		hr = dxgiFactory->EnumAdapterByGpuPreference(desc.adapterInfo->adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
	} else {
		hr = dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
	}
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(_d3dDevice.ptrForInit()));
	if (!Util::isValid(hr) && desc.useWarpDeviceFallback)
	{
		AXE_LOG_WARN("No D3D12 Adapter selected. Falling back to WARP (software) adapter");
		_dxgiAdapter.reset(nullptr);
		hr = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);

		hr = D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(_d3dDevice.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);
	}
	else {
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);
	}

#if defined(_DEBUG)
	{
		::DXGI_ADAPTER_DESC3 adapterDesc;
		_dxgiAdapter->GetDesc3(&adapterDesc);
		TempString tmp;
		UtfUtil::convert(tmp, adapterDesc.Description);
		setDebugName(tmp);
	}
#endif

#if defined(_DEBUG)
	{ // DRED settings persist beyond the interface lifetime
		ComPtr<DX12_ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dredSettings.ptrForInit()))))
		{
			dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
//			dredSettings->SetBreadcrumbContextEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON); // already removed, it's now automatic when called SetAutoBreadcrumbsEnablement
			AXE_LOG_WARN("DX12 DRED Enabled");
		}
	}
#endif

	_capabilities = new RenderCapabilities_DX12(this);
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12