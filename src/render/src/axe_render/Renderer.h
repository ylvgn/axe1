#pragma once

#include "RenderDevice.h"

namespace axe {

class Renderer_CreateDesc : public RenderDevice_CreateDesc {
public:
	Renderer_CreateDesc() noexcept;

	bool multithread : 1;
}; // Renderer_CreateDesc


class Renderer : public NonCopyable {
	using This	 = Renderer;
	using Device = RenderDevice;
public:
	using CreateDesc = Renderer_CreateDesc;

	static Renderer* s_instance() { return _s_instance; };
	static Renderer* s_create(CreateDesc& desc);

	virtual ~Renderer() noexcept;

	bool	multithread() const { return _multithread; }

	const Span<const RenderAdapterInfo> adapterInfos() const { return _adapterInfos; };

	const RenderAdapterInfo* findAdapterInfo(int i = 0) const { return _adapterInfos.inBound(i) ? &_adapterInfos[i] : nullptr; }

	Device*		  findDevice(int i = 0) const { return _devices.inBound(i) ? _devices[i] : nullptr; }
	Span<Device*> devices() { return _devices; }

	RenderDevice* createRenderDevice(RenderDevice_CreateDesc& desc);

	void onRenderDeviceDestory(RenderDevice* device);

protected:
	Renderer(CreateDesc& desc) noexcept; // please create from Renderer::s_create

	virtual Device* onCreateRenderDevice(RenderDevice_CreateDesc& desc) = 0;

	Vector<RenderAdapterInfo, 1>	_adapterInfos;
	Vector<Device*, 1>				_devices;

private:
	static Renderer* _s_instance;
	bool _multithread : 1;
}; // Renderer

} // namespace axe