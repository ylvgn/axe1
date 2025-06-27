#pragma once

#include "RenderObject.h"

namespace axe {

#define RendererApi_ENUM_LIST(E) \
	E(Unknown, )                 \
	E(DX11, )                    \
	E(DX12, )                    \
	E(OpenGL, )                  \
	E(Metal, )                   \
//----
AXE_ENUM_CLASS(RendererApi, u8)


class RenderDevice_CreateDesc : public RenderObject_CreateDesc {
public:
	RenderDevice_CreateDesc() noexcept;
	RenderDevice_CreateDesc(const RenderAdapterInfo* info) noexcept;

	RendererApi api;

	const RenderAdapterInfo* adapterInfo = nullptr;

	bool vsync					: 1;
	bool useWarpDeviceFallback	: 1;
}; // RenderDevice_CreateDesc


class RenderDevice : public RenderObject {
	using Base = RenderObject;
	using This = RenderDevice;
public:
	using CreateDesc = RenderDevice_CreateDesc;

	void	setVSync(bool b)	{ _vsync = b; }
	bool	vsync()	const		{ return _vsync; }

	RendererApi			api()		   const { return _api; }
	RenderCapabilities* capabilities() const { return _capabilities; }

	SPtr<RenderContext> createContext(RenderContext_CreateDesc& desc);

	~RenderDevice();

protected:
	RenderDevice(CreateDesc& desc) noexcept; // please create from Renderer::createDevice

	virtual SPtr<RenderContext> onCreateContext(RenderContext_CreateDesc& desc) = 0;

	RendererApi _api = RendererApi::Unknown;

	RenderCapabilities* _capabilities = nullptr;

	bool _vsync	: 1;
}; // RenderDevice


#define axeRenderDevice_InterfaceFunctions(T)                                          \
	virtual SPtr<RenderContext> onCreateContext(RenderContext_CreateDesc& desc) final; \
//----

#define axeRenderDevice_InterfaceFunctions_Impl(T)                                        \
	SPtr<RenderContext> RenderDevice_##T::onCreateContext(RenderContext_CreateDesc& desc) \
	{                                                                                     \
		return new RenderContext_##T(desc);                                               \
	}                                                                                     \
//----

} // namespace axe

