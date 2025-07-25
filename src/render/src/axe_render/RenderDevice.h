#pragma once

#include "RenderDeviceObject.h"

namespace axe {

#define RendererApi_ENUM_LIST(E) \
	E(Unknown, )                 \
	E(DX11, )                    \
	E(DX12, )                    \
	E(OpenGL, )                  \
	E(Metal, )                   \
//----
AXE_ENUM_CLASS(RendererApi, u8)


class RenderDevice_CreateDesc {
public:
	RenderDevice_CreateDesc() noexcept;
	RenderDevice_CreateDesc(const RenderAdapterInfo* info) noexcept;

	RendererApi api;

	const RenderAdapterInfo* adapterInfo = nullptr;

	bool VSync					: 1;
	bool useWarpDeviceFallback	: 1;
}; // RenderDevice_CreateDesc


class RenderDevice : public RenderDeviceObject {
	AXE_ABSTRACT_CLASS_TYPE(RenderDevice, RenderDeviceObject)
public:
	using CreateDesc = RenderDevice_CreateDesc;

	~RenderDevice();

	void	setVSync(bool b)	{ _VSync = b; }
	bool	VSync()	const		{ return _VSync; }

	RendererApi			api()		   const { return _api; }
	RenderCapabilities* capabilities() const { return _capabilities; }

	SPtr<RenderContext>	  createContext(RenderContext_CreateDesc& desc);
	SPtr<RenderGpuBuffer> createGpuBuffer(RenderGpuBuffer_CreateDesc& desc);

protected:
	RenderDevice(CreateDesc& desc) noexcept; // please create from Renderer::createDevice

	virtual SPtr<RenderContext>   onCreateContext(RenderDevice* device, RenderContext_CreateDesc& desc) = 0;
	virtual SPtr<RenderGpuBuffer> onCreateGpuBuffer(RenderDevice* device, RenderGpuBuffer_CreateDesc& desc) = 0;

	RendererApi _api = RendererApi::Unknown;

	RenderCapabilities* _capabilities = nullptr;

	bool _VSync	: 1;
}; // RenderDevice


#define axeRenderDevice_InterfaceFunctions(T)                                                                  \
	virtual SPtr<RenderContext>	  onCreateContext(RenderDevice* device, RenderContext_CreateDesc& desc) final; \
	virtual SPtr<RenderGpuBuffer> onCreateGpuBuffer(RenderDevice* device, RenderGpuBuffer_CreateDesc& desc) final; \
//----

#define axeRenderDevice_InterfaceFunctions_Impl(T)                                                              \
	SPtr<RenderContext> Device_##T::onCreateContext(RenderDevice* device, RenderContext_CreateDesc& desc)       \
	{                                                                                                           \
		return new Context_##T(device, desc);                                                                   \
	}                                                                                                           \
	SPtr<RenderGpuBuffer> Device_##T::onCreateGpuBuffer(RenderDevice* device, RenderGpuBuffer_CreateDesc& desc) \
	{                                                                                                           \
		SPtr<RenderGpuBuffer> p = new GpuBuffer_##T();                                                          \
		p->_internalSetDevice(device);                                                                          \
		p->create(desc);                                                                                        \
		return p;                                                                                               \
	}                                                                                                           \
//----

} // namespace axe

