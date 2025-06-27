#if AXE_RENDER_HAS_DX12

#include "Renderer_DX12.h"
#include "RenderDevice_DX12.h"

namespace axe {

Renderer_DX12::Renderer_DX12(CreateDesc& desc)
	: Base(desc)
{
	::HRESULT hr;
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

#if defined(_DEBUG)
	ComPtr<ID3D12Debug1> _d3dDebug;
	hr = D3D12GetDebugInterface(IID_PPV_ARGS(_d3dDebug.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	// Enable the debug layer (requires the Graphics Tools "optional feature" from Windows Settings > System > Optional features).
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	_d3dDebug->EnableDebugLayer();

	// Optional: Enable GPU-Based Validation (GBV)
	ComPtr<DX12_ID3D12Debug> modernD3dDebug;
	hr = _d3dDebug.As(&modernD3dDebug);
	if (Util::isValid(hr)) {
		modernD3dDebug->SetEnableGPUBasedValidation(true);
	}
	else {
		_d3dDebug->SetEnableGPUBasedValidation(true);
	}
#endif

	hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_getHardwareAdapterBasicInfo();
}

RenderDevice_DX12* Renderer_DX12::findDevice(int i) const {
	return static_cast<RenderDevice_DX12*>(Base::findDevice(i));
}

DX12_ID3D12Device* Renderer_DX12::d3dDevice(int i) {
	auto* p = findDevice(i);
	return p ? p->d3dDevice() : nullptr;
}

RenderDevice* Renderer_DX12::onCreateRenderDevice(RenderDevice_CreateDesc& desc) {
	return new RenderDevice_DX12(desc);
}

void Renderer_DX12::_getHardwareAdapterBasicInfo() {
	AXE_ASSERT(_dxgiFactory != nullptr);

	::HRESULT hr;
	::DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED;

	static const auto kToMegaByte = 1.0f / Math::MSizeInBytes<SIZE_T>();

	for (UINT adapterIndex = 0;; ++adapterIndex) {
		ComPtr<IDXGIAdapter> dxgiAdapter;
		hr = _dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, gpuPreference, IID_PPV_ARGS(dxgiAdapter.ptrForInit()));

		if (DXGI_ERROR_NOT_FOUND == hr)
			break;

		ComPtr<DX12_IDXGIAdapter> adapter;
		hr = dxgiAdapter.As(&adapter);
		if (!Util::isValid(hr))
			continue;

		::DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);

		// Check adapter capabilities
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Skip software adapters
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12.0, but don't create the actual device yet.
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, _uuidof(decltype(adapter)), nullptr);
		if (!Util::isValid(hr))
			continue;

		auto& adapterInfo = _adapterInfos.emplace_back();
		adapterInfo.adapterIndex = adapterIndex;
		UtfUtil::convert(adapterInfo.adapterName, desc.Description);
		adapterInfo.memorySize = desc.DedicatedVideoMemory;

		AXE_LOG("DX12 Adpter = {}\n"
				"    SubSysId = {}\n"
				"    Revision = {}\n"
				"    VendorId = 0x{:0X}\n"
				"    DeviceId = 0x{:0X}\n"
				"    AdapterLuid = {}\n"
				"    Video  Memory = {}MB\n"
				"    System Memory = {}MB\n"
				"    Shared Memory = {}MB\n"
				"    Flags = 0x{:08X}\n"
				"    ===== Monitor Info =====",
				adapterInfo.adapterName
			  , desc.SubSysId
			  , desc.Revision
			  , desc.VendorId
			  , desc.DeviceId
			  , desc.AdapterLuid
			  , desc.DedicatedVideoMemory * kToMegaByte
			  , desc.DedicatedSystemMemory * kToMegaByte
			  , desc.SharedSystemMemory * kToMegaByte
			  , enumInt(desc.Flags)
		);

		UINT outputIndex = 0;
		ComPtr<IDXGIOutput> dxgiOutput;
		while (adapter->EnumOutputs(outputIndex++, dxgiOutput.ptrForInit()) == S_OK)
		{
			ComPtr<DX12_IDXGIOutput> output;
			hr = dxgiOutput.As(&output);
			if (!Util::isValid(hr))
				continue;

			::DXGI_OUTPUT_DESC1 outputDesc;
			output->GetDesc1(&outputDesc);

			Rect2f resolution;
			Util::convert(resolution, outputDesc.DesktopCoordinates);

			AXE_LOG("    Monitor {} - {}x{}\n"
					"        HDR: {}\n"
					"        BPP: {}\n"
					"        Min Lum: {}\n"
					"        Max Lum: {}\n"
					"        MaxFFL: {}\n",
				outputIndex, resolution.w, resolution.h
			  , outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "Yes" : "No"
			  , outputDesc.BitsPerColor
			  , outputDesc.MinLuminance
			  , outputDesc.MaxLuminance
			  , outputDesc.MaxFullFrameLuminance);
		}
	}
}

Renderer_DX12::LiveObjectReporter::~LiveObjectReporter() {
	ComPtr<DX12_IDXGIDebug> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiDebug.ptrForInit()))))
	{
		ComPtr<IDXGIInfoQueue> pInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(pInfoQueue.ptrForInit())))) {
			pInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
			AXE_ASSERT(pInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) == 0);
		}
	}
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12
