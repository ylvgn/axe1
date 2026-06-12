#include "Renderer.h"
#include "RenderDevice.h"
#include "RenderContext.h"
#include "buffer/RenderGpuBuffer.h"

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

RenderDevice::~RenderDevice() {
	auto* renderer = Renderer::s_instance();
	renderer->onRenderDeviceDestroy(this);
}

UPtr<RenderContext> RenderDevice::createContext(const RenderContext_CreateDesc& desc) {
	return onCreateContext(this, desc);
}

UPtr<RenderGpuBuffer> RenderDevice::createGpuBuffer(const RenderGpuBuffer_CreateDesc& desc) {
	return onCreateGpuBuffer(this, desc);
}

} // namespace axe
