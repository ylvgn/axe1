#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderDevice_CreateDesc : public Renderer_CreateDesc {
	AXE_TYPE_INFO(RenderDevice_CreateDesc, Renderer_CreateDesc)
public:
	RenderDevice_CreateDesc() noexcept;
	RenderDevice_CreateDesc(const RenderAdapterInfo& info) noexcept;

	const RenderAdapterInfo* adapterInfo = nullptr;
	bool 					 vsync					: 1;
	bool 					 useWarpDeviceFallback	: 1;

}; // RenderDevice_CreateDesc


class RenderDevice : public RenderObject {
	AXE_RTTI_INFO(RenderDevice, RenderObject)
public:
	using CreateDesc = RenderDevice_CreateDesc;

	~RenderDevice() noexcept;

	RenderCapabilities* capabilities()	const	{ return _capabilities; }

	RendererApi			api()			const	{ return _desc.api; }
	bool				vsync()			const	{ return _desc.vsync; }
	bool				multithread()	const	{ return _desc.multithread; }

protected:
	RenderDevice(CreateDesc& desc) noexcept; // please create from Renderer::createDevice

	RenderCapabilities* _capabilities = nullptr;
private:
	CreateDesc _desc;
}; // RenderDevice

} // namespace axe

