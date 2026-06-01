#include "Renderer.h"
#include "RenderDevice.h"

#include "backend/dx12/Renderer_DX12.h"

namespace axe {

Renderer* Renderer::_s_instance = nullptr;

Renderer_CreateDesc::Renderer_CreateDesc() noexcept
	: multithread(false)
{
}

Renderer* Renderer::s_create(CreateDesc& desc) {
	Renderer* p = nullptr;

	switch (desc.api)
	{
		#if AXE_RENDER_HAS_DX12
			case RendererApi::Dx12: p = new Renderer_DX12(desc); break;
		#endif
	//---
		default: AXE_THROW;
	}

	AXE_ASSERT(p->_adapterInfos.size() > 0);
	p->_devices.reserve(p->_adapterInfos.size());
	return p;
}

RenderDevice* Renderer::createRenderDevice(RenderDevice_CreateDesc& desc) {
	auto& device = _devices.emplace_back(onCreateRenderDevice(desc));
	AXE_ASSERT(device->capabilities() != nullptr);
	return device;
}

void Renderer::onRenderDeviceDestroy(RenderDevice* device) {
	auto it = ::eastl::find(_devices.begin(), _devices.end(), device);
	if (it != _devices.end()) {
		_devices.erase_unsorted(it);
	}
}

Renderer::Renderer(CreateDesc& desc) noexcept
	: _multithread(desc.multithread)
{
	AXE_ASSERT(_s_instance == nullptr);
	_s_instance = this;
}

RenderDevice* Renderer::s_rootDevice() {
	return _s_instance->findDevice(0);
}

Renderer::~Renderer() noexcept
{
	AXE_ASSERT(_s_instance == this);
	AXE_ASSERT(_devices.size() == 0);

	_s_instance = nullptr;
}

} // namespace axe