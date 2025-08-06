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
	: VSync(true)
	, useWarpDeviceFallback(false)
	, adapterInfo(nullptr)
{

#if AXE_OS_WINDOWS
	api = RendererApi::DX12;
#elif AXE_OS_OSX || AXE_OS_IOS
	api = RendererApi::Metal;
#elif AXE_OS_LINUX
	api = RendererApi::OpenGL;
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
