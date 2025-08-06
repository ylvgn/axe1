#if AXE_RENDER_HAS_DX12

#include "Device_DX12.h"
#include "Context_DX12.h"
#include "Renderer_DX12.h"
#include "Capabilities_DX12.h"
#include "GpuBuffer_DX12.h"
#include "Fence_DX12.h"

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

		hr = dxgiFactory->EnumAdapterByLuid(targetAdapterLuid, IID_PPV_ARGS(_dxgiAdapter.ptrForInit()));
		switch (hr) {
			case S_OK:
				AXE_LOG("Choosed prefer Adapter LUID {}", targetAdapterLuid);
				break;
			case DXGI_ERROR_NOT_FOUND:
				AXE_LOG_WARN("DX12 prefer Adapter LUID {} not found", targetAdapterLuid);
				break;
			default:
				AXE_DX12_THROWIF_HRESULT_ERROR(hr);
		}
	}
	if (!_dxgiAdapter) {
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

#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(_d3dDevice->QueryInterface(IID_PPV_ARGS(pInfoQueue.ptrForInit()))))
	{
		hr = pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, _d3dDevice);

		hr = pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, _d3dDevice);

		hr = pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, _d3dDevice);

		AXE_LOG_WARN("Enabled DX12 Validation Break on Severity");

		::D3D12_MESSAGE_ID ignoreIDs[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
		};

		::D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs	= ARRAYSIZE(ignoreIDs);
		filter.DenyList.pIDList = ignoreIDs;
		pInfoQueue->AddStorageFilterEntries(&filter);

		ComPtr<ID3D12InfoQueue1> pInfoQueue1;
		hr = pInfoQueue.As(&pInfoQueue1);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, _d3dDevice);

		::DWORD callbackCookie = 0;
		hr = pInfoQueue1->RegisterMessageCallback(
			s_D3D12MessageFunc,
			D3D12_MESSAGE_CALLBACK_FLAG_NONE,
			this,
			&callbackCookie
		);
		AXE_DX12_THROWIF_HRESULT_ERROR(hr, _d3dDevice);
	}
#endif
}

void Device_DX12::onD3D12MessageCallback(::D3D12_MESSAGE_CATEGORY category
									   , ::D3D12_MESSAGE_SEVERITY severity
									   , ::D3D12_MESSAGE_ID id
									   , StrView description) {
	switch (id) {
		case D3D12_MESSAGE_ID_LIVE_OBJECT_SUMMARY:
		case D3D12_MESSAGE_ID_LIVE_DEVICE:
		case D3D12_MESSAGE_ID_LIVE_SWAPCHAIN:
		case D3D12_MESSAGE_ID_LIVE_COMMANDQUEUE:
		case D3D12_MESSAGE_ID_LIVE_COMMANDALLOCATOR:
		case D3D12_MESSAGE_ID_LIVE_PIPELINESTATE:
		case D3D12_MESSAGE_ID_LIVE_COMMANDLIST12:
		case D3D12_MESSAGE_ID_LIVE_RESOURCE:
		case D3D12_MESSAGE_ID_LIVE_DESCRIPTORHEAP:
		case D3D12_MESSAGE_ID_LIVE_ROOTSIGNATURE:
		case D3D12_MESSAGE_ID_LIVE_LIBRARY:
		case D3D12_MESSAGE_ID_LIVE_HEAP:
		case D3D12_MESSAGE_ID_LIVE_MONITOREDFENCE:
		case D3D12_MESSAGE_ID_LIVE_QUERYHEAP:
		case D3D12_MESSAGE_ID_LIVE_COMMANDSIGNATURE:
		case D3D12_MESSAGE_ID_LIVE_PIPELINELIBRARY:
		case D3D12_MESSAGE_ID_LIVE_VIDEODECODECOMMANDLIST:
		case D3D12_MESSAGE_ID_LIVE_VIDEODECODER:
		case D3D12_MESSAGE_ID_LIVE_VIDEODECODESTREAM:
		case D3D12_MESSAGE_ID_LIVE_VIDEODECODECOMMANDQUEUE:
		case D3D12_MESSAGE_ID_LIVE_VIDEOPROCESSCOMMANDLIST:
		case D3D12_MESSAGE_ID_LIVE_VIDEOPROCESSCOMMANDQUEUE:
		case D3D12_MESSAGE_ID_LIVE_VIDEOPROCESSOR:
		case D3D12_MESSAGE_ID_LIVE_VIDEOPROCESSSTREAM:
		case D3D12_MESSAGE_ID_LIVE_CRYPTO_SESSION:
		case D3D12_MESSAGE_ID_LIVE_CRYPTO_SESSION_POLICY:
		case D3D12_MESSAGE_ID_LIVE_PROTECTED_RESOURCE_SESSION:
		case D3D12_MESSAGE_ID_LIVE_VIDEODECODERHEAP:
		case D3D12_MESSAGE_ID_LIVE_COMMANDRECORDER:
		case D3D12_MESSAGE_ID_LIVE_COMMANDPOOL:
		case D3D12_MESSAGE_ID_LIVE_META_COMMAND:
		case D3D12_MESSAGE_ID_LIVE_LIFETIMETRACKER:
		case D3D12_MESSAGE_ID_LIVE_TRACKEDWORKLOAD:
		case D3D12_MESSAGE_ID_LIVE_VIDEOENCODECOMMANDLIST:
		case D3D12_MESSAGE_ID_LIVE_VIDEOENCODECOMMANDQUEUE:
		case D3D12_MESSAGE_ID_LIVE_VIDEOMOTIONESTIMATOR:
		case D3D12_MESSAGE_ID_LIVE_VIDEOMOTIONVECTORHEAP:
		case D3D12_MESSAGE_ID_LIVE_VIDEOEXTENSIONCOMMAND:
		case D3D12_MESSAGE_ID_LIVE_SHADERCACHESESSION:
		case D3D12_MESSAGE_ID_LIVE_VIDEOENCODER:
		case D3D12_MESSAGE_ID_LIVE_VIDEOENCODERHEAP:
		{
			AXE_LOG_WARN("D3D12 Resource Leak: {}", description);
			const char* pFindResult = strstr(description.data(), "Live ID3D12Resource at 0x");
			if (nullptr != pFindResult)
			{
				intptr_t address;
				if (sscanf_s(pFindResult, "Live ID3D12Resource at 0x%llx", &address) == 1)
				{
					auto* resource  = reinterpret_cast<::ID3D12Resource*>(address);
					Callstack<6>	callstack;
					if (DX12Util::getResourceCallstack(callstack, resource)) {
						AXE_LOG_WARN("D3D12 Resource Callstack:\n {}\n", callstack.ToString());
					}
				}
			}
		} break;
		default: AXE_LOG_WARN("D3D12 Validation Layer: {}", description); break;
	} // switch end
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12