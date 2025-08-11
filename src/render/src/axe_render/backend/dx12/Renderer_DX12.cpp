#if AXE_RENDER_HAS_DX12

#include "Renderer_DX12.h"
#include "Device_DX12.h"

namespace axe {

Renderer_DX12::Renderer_DX12(CreateDesc& desc)
	: Base(desc)
{
	::HRESULT hr;
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	{
#if defined(_DEBUG)
		ComPtr<ID3D12Debug> d3dDebug;
		hr = ::D3D12GetDebugInterface(IID_PPV_ARGS(d3dDebug.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);

		hr = d3dDebug->QueryInterface(IID_PPV_ARGS(_d3dDebug.ptrForInit()));
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);
#endif
		// Optional: Enable Debug Layer
		setDebugLayer(true);
		// Optional: Enable GPU-Based Validation (GBV)
		setGpuBasedValidation(true);
		// Optional: Enable detect synchronization issues between multiple command queues.
		setSyncCommandQueueValidation(true);
	}

	hr = ::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(_dxgiFactory.ptrForInit()));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_getHardwareAdapterBasicInfo();
}

void Renderer_DX12::setDebugLayer(bool isEnable) {
#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature" from Windows Settings > System > Optional features).
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	if (isEnable) {
		_d3dDebug->EnableDebugLayer();
	} else {
		ComPtr<ID3D12Debug4> d3dDebug4;
		if (SUCCEEDED(_d3dDebug->QueryInterface(IID_PPV_ARGS(d3dDebug4.ptrForInit())))) {
			d3dDebug4->DisableDebugLayer(); // require ID3D12Debug4
		}
	}
#endif
}

void Renderer_DX12::setGpuBasedValidation(bool isEnable, ::D3D12_GPU_BASED_VALIDATION_FLAGS flags) {
#if defined(_DEBUG)
	AXE_ASSERT(_d3dDebug != nullptr);

	_d3dDebug->SetEnableGPUBasedValidation(isEnable); // require ID3D12Debug1
	

	{ // set flags
		ComPtr<ID3D12Debug3> d3dDebug3;
		if (SUCCEEDED(_d3dDebug->QueryInterface(IID_PPV_ARGS(d3dDebug3.ptrForInit())))) {
			d3dDebug3->SetGPUBasedValidationFlags(flags); // require ID3D12Debug3
		}
	}
#endif
}

void Renderer_DX12::setSyncCommandQueueValidation(bool isEnable) {
#if defined(_DEBUG)
	_d3dDebug->SetEnableSynchronizedCommandQueueValidation(isEnable);
#endif
}

void Renderer_DX12::Helper::forEachDXGIAdapter(ForEachDXGIAdapterHandler func) {
	AXE_ASSERT(func != nullptr);

	auto* renderer	  = Renderer_DX12::s_instance();
	auto* dxgiFactory = renderer->dxgiFactory();
	AXE_ASSERT(dxgiFactory != nullptr);

	::HRESULT			  hr;
	::DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED;

	for (::UINT i = 0;; ++i) {
		ComPtr<IDXGIAdapter> dxgiAdapter;
		hr = dxgiFactory->EnumAdapterByGpuPreference(i, gpuPreference, IID_PPV_ARGS(dxgiAdapter.ptrForInit()));

		if (DXGI_ERROR_NOT_FOUND == hr)
			break;

		if (!Util::isValid(hr))
		{
			AXE_LOG_ERROR("[forEachDXGIAdapter] error: {}", DX12_HRESULT_String(hr, nullptr));
			continue;
		}

		if ( func(dxgiAdapter) ) {
			// return true will early exit
			break;
		}
	}
}

void Renderer_DX12::_getHardwareAdapterBasicInfo() {
	Helper::forEachDXGIAdapter([this](IDXGIAdapter* dxgiAdapter) {
		::HRESULT hr;
		static const auto kToMegaByte = 1.0f / Math::MSizeInBytes<SIZE_T>();

		ComPtr<DX12_IDXGIAdapter> adapter;
		hr = dxgiAdapter->QueryInterface(IID_PPV_ARGS(adapter.ptrForInit()));
		if (!Util::isValid(hr))
			return false;

		::DXGI_ADAPTER_DESC3 desc;
		adapter->GetDesc3(&desc);

		// Check adapter capabilities
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Skip software adapters
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			return false;
		}

		// Check to see if the adapter supports Direct3D 12.0, but don't create the actual device yet.
		hr = ::D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, _uuidof(decltype(adapter)), nullptr);
		if (!Util::isValid(hr))
			return false;

		auto& adapterInfo = _adapterInfos.emplace_back();
		Util::convert(adapterInfo.LUID, desc.AdapterLuid);
		UtfUtil::convert(adapterInfo.adapterName, desc.Description);
		adapterInfo.memorySize = desc.DedicatedVideoMemory;

		AXE_LOG("DX12 Adapter = {}\n"
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
			  , desc.DedicatedVideoMemory  * kToMegaByte
			  , desc.DedicatedSystemMemory * kToMegaByte
			  , desc.SharedSystemMemory    * kToMegaByte
			  , enumInt(desc.Flags)
		);

		UINT outputIndex = 0;
		ComPtr<IDXGIOutput> dxgiOutput;
		while (adapter->EnumOutputs(outputIndex++, dxgiOutput.ptrForInit()) == S_OK)
		{
			ComPtr<DX12_IDXGIOutput> output;
			if (!dxgiOutput.As(&output))
				continue;

			::DXGI_OUTPUT_DESC1 outputDesc;
			output->GetDesc1(&outputDesc);

			Rect2f desktopCoordinates;
			Util::convert(desktopCoordinates, outputDesc.DesktopCoordinates);

			AXE_LOG("    Monitor {} - {}x{}\n"
					"        Virtual Screen Pos: ({}, {})\n"
					"        HDR: {}\n"
					"        BPP: {}\n"
					"        Min Lum: {}\n"
					"        Max Lum: {}\n"
					"        MaxFFL: {}\n",
				outputIndex
			  , desktopCoordinates.w, desktopCoordinates.h
			  , desktopCoordinates.x, desktopCoordinates.y
			  , outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 ? "Yes" : "No"
			  , outputDesc.BitsPerColor
			  , outputDesc.MinLuminance
			  , outputDesc.MaxLuminance
			  , outputDesc.MaxFullFrameLuminance);
		}

		return false;
	});
}

Renderer_DX12::LiveObjectReporter::~LiveObjectReporter() {
	ComPtr<DX12_IDXGIDebug> dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiDebug.ptrForInit()))))
	{
		ComPtr<IDXGIInfoQueue> pInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(pInfoQueue.ptrForInit())))) {
			pInfoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			AXE_ASSERT(pInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) == 0);
		}
	}
}

Device_DX12* Renderer_DX12::findDevice(int i) const {
	return static_cast<Device_DX12*>(Base::findDevice(i));
}

Span< Device_DX12* > Renderer_DX12::devices() {
	using DST = Device_DX12*;
	return Span<DST>(reinterpret_cast<DST*>(_devices.data()), _devices.size());
}

DX12_ID3D12Device* Renderer_DX12::d3dDevice(int i) {
	auto* p = findDevice(i);
	return p ? p->d3dDevice() : nullptr;
}

RenderDevice* Renderer_DX12::onCreateRenderDevice(RenderDevice_CreateDesc& desc) {
	return new Device_DX12(desc);
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12
