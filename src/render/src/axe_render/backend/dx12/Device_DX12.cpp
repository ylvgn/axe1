#if AXE_RENDER_HAS_DX12

#include "Device_DX12.h"
#include "Context_DX12.h"
#include "Renderer_DX12.h"
#include "Capabilities_DX12.h"
#include "GpuBuffer_DX12.h"

namespace axe {

axeRenderDevice_InterfaceFunctions_Impl(DX12)

Device_DX12::Device_DX12(CreateDesc& desc)
	: Base(desc)
{
	::HRESULT hr;

	auto* renderer	  = Renderer_DX12::s_instance();
	auto* dxgiFactory = renderer->dxgiFactory();

	if (desc.adapterInfo) {
		::LUID targetAdapterLuid;
		Util::convert(targetAdapterLuid, desc.adapterInfo->LUID);
		Renderer_DX12::Helper::forEachDXGIAdapter([this, targetAdapterLuid](IDXGIAdapter* dxgiAdapter) {
			::HRESULT hr;
			
			ComPtr<DX12_IDXGIAdapter> adapter;
			hr = dxgiAdapter->QueryInterface(IID_PPV_ARGS(adapter.ptrForInit()));
			if (!Util::isValid(hr))
				return false;

			::DXGI_ADAPTER_DESC3 desc;
			adapter->GetDesc3(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				return false;
			}

			if (desc.AdapterLuid != targetAdapterLuid) {
				return false;
			}

			_dxgiAdapter = AXE_MOVE(adapter);
			return true;
		});
	} else {
		hr = dxgiFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);
	}
	AXE_ASSERT(_dxgiAdapter != nullptr);

	hr = ::D3D12CreateDevice(_dxgiAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(_d3dDevice.ptrForInit()));
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

	_capabilities = new Capabilities_DX12(this);
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12