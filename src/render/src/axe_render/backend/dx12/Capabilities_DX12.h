#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderCapabilities.h>
#include "Common_DX12.h"

namespace axe {
/*
- Refs: 
  - d3dx12_check_feature_support.h
*/

class Capabilities_DX12 : public RenderCapabilities {
	using This = Capabilities_DX12;
	using Base = RenderCapabilities;
	using Util = DX12Util;
public:
	Capabilities_DX12(Device_DX12* device);

	::D3D_FEATURE_LEVEL _d3dMaxFeatureLevel = static_cast<::D3D_FEATURE_LEVEL>(0);
private:
	void _checkTearing(DX12_IDXGIFactory* dxgiFactory);

	void _queryOptions  (DX12_ID3D12Device* device);
	void _queryOptions1 (DX12_ID3D12Device* device);
	void _queryOptions2 (DX12_ID3D12Device* device);
	void _queryOptions3 (DX12_ID3D12Device* device);
	void _queryOptions4 (DX12_ID3D12Device* device);
	void _queryOptions5 (DX12_ID3D12Device* device);
	void _queryOptions6 (DX12_ID3D12Device* device);
	void _queryOptions7 (DX12_ID3D12Device* device);
	void _queryOptions8 (DX12_ID3D12Device* device);
	void _queryOptions9 (DX12_ID3D12Device* device);
	void _queryOptions10(DX12_ID3D12Device* device);
	void _queryOptions11(DX12_ID3D12Device* device);
	void _queryOptions12(DX12_ID3D12Device* device);
	void _queryOptions13(DX12_ID3D12Device* device);
	void _queryOptions14(DX12_ID3D12Device* device);
	void _queryOptions15(DX12_ID3D12Device* device);
	void _queryOptions16(DX12_ID3D12Device* device);
	void _queryOptions17(DX12_ID3D12Device* device);
	void _queryOptions18(DX12_ID3D12Device* device);
	void _queryOptions19(DX12_ID3D12Device* device);
	void _queryOptions20(DX12_ID3D12Device* device);
	void _queryOptions21(DX12_ID3D12Device* device);

	void _queryHighestShaderModel(DX12_ID3D12Device* device);
	void _queryHighestFeatureLevel(DX12_ID3D12Device* device);

}; // Capabilities_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12