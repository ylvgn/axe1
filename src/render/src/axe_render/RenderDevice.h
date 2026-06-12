#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderDevice_CreateDesc : public Renderer_CreateDesc {
public:
	RenderDevice_CreateDesc() noexcept;
	RenderDevice_CreateDesc(const RenderAdapterInfo* info) noexcept;

	const RenderAdapterInfo* adapterInfo = nullptr;

	bool vsync					: 1;
	bool useWarpDeviceFallback	: 1;
}; // RenderDevice_CreateDesc


class RenderDevice : public RenderDeviceObject {
	AXE_RTTI_INFO(RenderDevice, RenderDeviceObject)
public:
	using CreateDesc = RenderDevice_CreateDesc;

	~RenderDevice();

	RenderCapabilities* capabilities()	const	{ return _capabilities; }

	RendererApi			api()			const	{ return _desc.api; }
	bool				vsync()			const	{ return _desc.vsync; }
	bool				multithread()	const	{ return _desc.multithread; }

	UPtr<RenderContext>	  createContext(const RenderContext_CreateDesc& desc);
	UPtr<RenderGpuBuffer> createGpuBuffer(const RenderGpuBuffer_CreateDesc& desc);

protected:
	RenderDevice(CreateDesc& desc) noexcept; // please create from Renderer::createDevice

	virtual UPtr<RenderContext>	  onCreateContext(RenderDevice* device, const RenderContext_CreateDesc& desc) = 0;
	virtual UPtr<RenderGpuBuffer> onCreateGpuBuffer(RenderDevice* device, const RenderGpuBuffer_CreateDesc& desc) = 0;

	RenderCapabilities* _capabilities = nullptr;

	CreateDesc _desc;
}; // RenderDevice


#define axeRenderDevice_InterfaceFunctions(T)                                                                  \
	virtual UPtr<RenderContext>	  onCreateContext(RenderDevice* device, const RenderContext_CreateDesc& desc) final; \
	virtual UPtr<RenderGpuBuffer> onCreateGpuBuffer(RenderDevice* device, const RenderGpuBuffer_CreateDesc& desc) final; \
//----

#define axeRenderDevice_InterfaceFunctions_Impl(T)                                                              \
	UPtr<RenderContext> Device_##T::onCreateContext(RenderDevice* device, const RenderContext_CreateDesc& desc)       \
	{                                                                                                           \
		return UPtr<RenderContext>(new Context_##T(device, desc));                                                      \
	}                                                                                                           \
	UPtr<RenderGpuBuffer> Device_##T::onCreateGpuBuffer(RenderDevice* device, const RenderGpuBuffer_CreateDesc& desc) \
	{                                                                                                           \
		UPtr<RenderGpuBuffer> p(new GpuBuffer_##T(device));                                                    \
		p->create(desc);                                                                                        \
		return p;                                                                                               \
	}                                                                                                           \
//----

} // namespace axe

