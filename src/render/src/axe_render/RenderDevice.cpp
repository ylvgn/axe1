#include "RenderDevice.h"
#include "Renderer.h"

namespace axe {

RenderDevice_CreateDesc::RenderDevice_CreateDesc() noexcept
	: adapterInfo(nullptr) 
	, vsync(true)
	, useWarpDeviceFallback(false)
{
}

RenderDevice_CreateDesc::RenderDevice_CreateDesc(const RenderAdapterInfo* info) noexcept
	: RenderDevice_CreateDesc()
{
	adapterInfo = info;
}

RenderDevice::RenderDevice(CreateDesc& desc) noexcept
	: _desc(desc)
{
}

RenderDevice::~RenderDevice() noexcept {
	auto* renderer = Renderer::s_instance();
	renderer->onRenderDeviceDestroy(this);
}

} // namespace axe
