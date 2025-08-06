#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/Renderer.h>
#include "Common_DX12.h"

namespace axe {

class Renderer_DX12 : public Renderer {
	using This = Renderer_DX12;
	using Base = Renderer;
	using Util = DX12Util;
public:
	AXE_DOWNCAST_GET_INSTANCE()

	Renderer_DX12(CreateDesc& desc);

	virtual RenderDevice* onCreateRenderDevice(RenderDevice_CreateDesc& desc) final;

	Device_DX12*	   findDevice(int i) const;
	Span<Device_DX12*> devices();

	DX12_IDXGIFactory* dxgiFactory() { return _dxgiFactory; }
	DX12_ID3D12Device* d3dDevice(int i = 0);

	struct Helper : public StaticAbstructClass {
		using ForEachDXGIAdapterHandler = AXE_FUNC<bool(IDXGIAdapter*)>;

		static void forEachDXGIAdapter(ForEachDXGIAdapterHandler func);
	}; // Helper

	void setDebugLayer(bool isEnable);
	void setGpuBasedValidation(bool isEnable, ::D3D12_GPU_BASED_VALIDATION_FLAGS flags = D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);
	void setSyncCommandQueueValidation(bool isEnable);

#if defined(_DEBUG)
	DX12_ID3D12Debug*  d3dDebug() { return _d3dDebug; }
#endif

private:
	struct LiveObjectReporter
	{
		// At application shutdown will auto called this destructor, for tracking DXGI/D3D resource leaks
		~LiveObjectReporter();
	} _internalReporter;

	void _getHardwareAdapterBasicInfo();

	ComPtr<DX12_IDXGIFactory> _dxgiFactory;
#if defined(_DEBUG)
	ComPtr<DX12_ID3D12Debug>  _d3dDebug;
#endif
}; // Renderer_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12
