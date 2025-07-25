#if AXE_RENDER_HAS_DX12

#include "Capabilities_DX12.h"
#include "Device_DX12.h"

namespace axe {

Capabilities_DX12::Capabilities_DX12(Device_DX12* device) {
	auto* d3dFactory = Util::dxgiFactory();
	{
		_checkTearing(d3dFactory);
	}

	auto* d3dDevice = device->d3dDevice();
	{
		_queryHighestFeatureLevel(d3dDevice);
		_queryHighestShaderModel(d3dDevice);

		if (_d3dMaxFeatureLevel >= D3D_FEATURE_LEVEL_12_0) {
			_queryOptions  (d3dDevice);
			_queryOptions1 (d3dDevice);
			_queryOptions2 (d3dDevice);
			_queryOptions3 (d3dDevice);
			_queryOptions4 (d3dDevice);
		}
		if (_d3dMaxFeatureLevel >= D3D_FEATURE_LEVEL_12_1) {
			_queryOptions5 (d3dDevice);
			_queryOptions6 (d3dDevice);
			_queryOptions7 (d3dDevice);
		}
		if (_d3dMaxFeatureLevel >= D3D_FEATURE_LEVEL_12_2) {
			_queryOptions8 (d3dDevice);
			_queryOptions9 (d3dDevice);
			_queryOptions10(d3dDevice);
			_queryOptions11(d3dDevice);
			_queryOptions12(d3dDevice);
			_queryOptions13(d3dDevice);
			_queryOptions14(d3dDevice);
			_queryOptions15(d3dDevice);
			_queryOptions16(d3dDevice);
			_queryOptions17(d3dDevice);
			_queryOptions18(d3dDevice);
			_queryOptions19(d3dDevice);
			_queryOptions20(d3dDevice);
			_queryOptions21(d3dDevice);
		}
	}
}

void Capabilities_DX12::_checkTearing(DX12_IDXGIFactory* dxgiFactory) {
	::HRESULT hr;

	BOOL allowTearingFlag = FALSE;
	hr = dxgiFactory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING
										, &allowTearingFlag
										, sizeof(allowTearingFlag));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);
	_info.hasTearing = static_cast<bool>(allowTearingFlag);
}

void Capabilities_DX12::_queryOptions(DX12_ID3D12Device* device)
{
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS op = {};

	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_info.shaderHasFloat64 = op.DoublePrecisionFloatShaderOps;
}

void Capabilities_DX12::_queryOptions1(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS1 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
}

void Capabilities_DX12::_queryOptions2(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS2 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
}

void Capabilities_DX12::_queryOptions3(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS3 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS3, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
}

void Capabilities_DX12::_queryOptions4(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS4 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
}

void Capabilities_DX12::_queryOptions5(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS5 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_info.hasRaytracing = op.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

void Capabilities_DX12::_queryOptions6(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS6 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
}

void Capabilities_DX12::_queryOptions7(DX12_ID3D12Device* device) {
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS7 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	_info.hasMeshShader = op.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
}

void Capabilities_DX12::_queryOptions8(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 3)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS8 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS8, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions9(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 3)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS9 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS9, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions10(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 4)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS10 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS10, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions11(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 4)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS11 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS11, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions12(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 600)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS12 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions13(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 602)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS13 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS13, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions14(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 606)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS14 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS14, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions15(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 606)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS15 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS15, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions16(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 608)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS16 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS16, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions17(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 609)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS17 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS17, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions18(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 609)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS18 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS18, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions19(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 610)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS19 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS19, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions20(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 611)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS20 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS20, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryOptions21(DX12_ID3D12Device* device) {
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 612)
	::HRESULT hr;
	::D3D12_FEATURE_DATA_D3D12_OPTIONS21 op = {};
	hr = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS21, &op, sizeof(op));
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	AXE_UNUSED(op);
#endif
}

void Capabilities_DX12::_queryHighestShaderModel(DX12_ID3D12Device* device) {
    ::HRESULT hr;
	::D3D12_FEATURE_DATA_SHADER_MODEL s = {};

    static const D3D_SHADER_MODEL s_shaderModels[] = {
		// Check support in descending order
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 612)
        D3D_SHADER_MODEL_6_9,
#endif
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 606)
        D3D_SHADER_MODEL_6_8,
#endif
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 3)
        D3D_SHADER_MODEL_6_7,
#endif
        D3D_SHADER_MODEL_6_6,
        D3D_SHADER_MODEL_6_5,
        D3D_SHADER_MODEL_6_4,
        D3D_SHADER_MODEL_6_3,
        D3D_SHADER_MODEL_6_2,
        D3D_SHADER_MODEL_6_1,
        D3D_SHADER_MODEL_6_0,
        D3D_SHADER_MODEL_5_1
    };
	const constexpr size_t kNumModelVersions = ARRAYSIZE(s_shaderModels);

	s.HighestShaderModel = static_cast<D3D_SHADER_MODEL>(0);
    for (size_t i = 0; i < kNumModelVersions; i++) {
		s.HighestShaderModel = s_shaderModels[i];
        hr = device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &s, sizeof(s));

        if (hr != E_INVALIDARG)
        {
            // Indicates that the version is recognizable by the runtime and stored in the struct
            // Also terminate on unexpected error code
            if (FAILED(hr))
            {
				s.HighestShaderModel = static_cast<D3D_SHADER_MODEL>(0);
            }
            break;
        }
    }
	AXE_DX12_THROWIF_HRESULT_ERROR(hr);

	const auto kReqShaderModel = D3D_SHADER_MODEL_6_0;
	if (s.HighestShaderModel < kReqShaderModel)
	{
		AXE_LOG_WARN("DX12 supported shader model 0x{:0X} < 0x{:0X} is required for DXIL shader",
			static_cast<int>(s.HighestShaderModel), static_cast<int>(kReqShaderModel)
		);
	}

	if (s.HighestShaderModel >= D3D_SHADER_MODEL_5_1)
	{
		_info.hasComputeShader = true;
	}
}

void Capabilities_DX12::_queryHighestFeatureLevel(DX12_ID3D12Device* device)
{
	::HRESULT hr;

	// Check against a list of all feature levels present in d3dcommon.h
	// Needs to be updated for future feature levels
	static const ::D3D_FEATURE_LEVEL s_featureLevels[] = {
		// Check support in descending order
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 3)
		D3D_FEATURE_LEVEL_12_2,
#endif
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 5)
		D3D_FEATURE_LEVEL_1_0_CORE,
#endif
#if defined(D3D12_SDK_VERSION) && (D3D12_SDK_VERSION >= 611)
		D3D_FEATURE_LEVEL_1_0_GENERIC
#endif
	};

	::D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {
		ARRAYSIZE(s_featureLevels),
		s_featureLevels,
		D3D_FEATURE_LEVEL_11_0
	};

	hr = device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
	if (hr == DXGI_ERROR_UNSUPPORTED)
	{
		AXE_LOG_WARN("DX12 none supported feature level: {}", hr);
		_d3dMaxFeatureLevel = static_cast<::D3D_FEATURE_LEVEL>(0);
	}
	else
	{
		AXE_DX12_THROWIF_HRESULT_ERROR(hr);
		_d3dMaxFeatureLevel = featLevels.MaxSupportedFeatureLevel;
	}
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12