#include "Renderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"
#include "RenderFence.h"

namespace axe {

RenderDevice_CreateDesc::RenderDevice_CreateDesc() noexcept
	: vsync(true)
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

SPtr<RenderContext> RenderDevice::createContext(RenderContext_CreateDesc& desc) {
	if (!desc.device) desc.device = this;
	return onCreateContext(desc);
}

RenderDevice::~RenderDevice() {
	auto* renderer = Renderer::s_instance();
	renderer->onRenderDeviceDestory(this);
}

RenderDevice::RenderDevice(CreateDesc& desc) noexcept
	: Base(desc)
	, _api(desc.api)
	, _vsync(desc.vsync)
{
}

} // namespace axe
