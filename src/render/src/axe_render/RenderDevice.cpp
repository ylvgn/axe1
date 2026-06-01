#include "Renderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"
#include "buffer/RenderGpuBuffer.h"

namespace axe {

const TypeInfo* RenderDevice::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderDevice";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

RenderDevice_CreateDesc::RenderDevice_CreateDesc() noexcept
	: adapterInfo(nullptr) 
	, VSync(true)
	, useWarpDeviceFallback(false)
{

#if AXE_OS_WINDOWS
	api = RendererApi::Dx12;
#else
	api = RendererApi::Vk;
#endif
}

RenderDevice_CreateDesc::RenderDevice_CreateDesc(const RenderAdapterInfo* info) noexcept
	: RenderDevice_CreateDesc()
{
	adapterInfo = info;
}

RenderDevice::~RenderDevice() {
	auto* renderer = Renderer::s_instance();
	renderer->onRenderDeviceDestroy(this);
}

SPtr<RenderContext> RenderDevice::createContext(RenderContext_CreateDesc& desc) {
	return onCreateContext(this, desc);
}

SPtr<RenderGpuBuffer> RenderDevice::createGpuBuffer(RenderGpuBuffer_CreateDesc& desc) {
	return onCreateGpuBuffer(this, desc);
}

RenderDevice::RenderDevice(CreateDesc& desc) noexcept
	: _api(desc.api)
	, _VSync(desc.VSync)
{
}

} // namespace axe
