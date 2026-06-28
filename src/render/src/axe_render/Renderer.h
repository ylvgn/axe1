#pragma once

#include "RenderCommonBase.h"

namespace axe {

class Renderer_CreateDesc {
public:
	Renderer_CreateDesc() noexcept;

	RendererApi api;
	bool multithread : 1;
}; // Renderer_CreateDesc

class Renderer : public RenderObject {
	AXE_RTTI_INFO(Renderer, RenderObject)
public:
	using CreateDesc = Renderer_CreateDesc;
	
	virtual ~Renderer() noexcept override;

	static Renderer*	 s_create(const CreateDesc& desc);

	static Renderer*	 s_instance();
	static RenderDevice* s_rootDevice() { auto* t = s_instance(); return t ? t->findDevice(0) : nullptr; }

	bool	multithread() const { return _multithread; }

	const Span<const RenderAdapterInfo> adapterInfos() const { return _adapterInfos; }

	const RenderAdapterInfo* findAdapterInfo(int i = 0) const { return _adapterInfos.inBound(i) ? &_adapterInfos[i] : nullptr; }

	RenderDevice* findDevice(int i) const { return _devices.inBound(i) ? _devices[i] : nullptr; }
	Span<RenderDevice*> devices() { return _devices; }

	RenderDevice*	createRenderDevice(RenderDevice_CreateDesc& desc);
	void			onRenderDeviceDestroy(RenderDevice* device);

	AXE_RenderObject_LIST(AXE_RenderSystem_NewObject, AXE_EMPTY, =0)
	
protected:
	Renderer(const CreateDesc& desc) noexcept; // please create from Renderer::s_create

	virtual RenderDevice* onCreateRenderDevice(RenderDevice_CreateDesc& desc) = 0;

	Vector<RenderAdapterInfo, 1> _adapterInfos;
	Vector<RenderDevice*, 1>	 _devices;

private:
	bool _multithread : 1;
}; // Renderer

} // namespace axe