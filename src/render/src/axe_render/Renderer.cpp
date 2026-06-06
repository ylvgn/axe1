#include "Renderer.h"
#include "RenderDevice.h"

#include "backend/dx12/Renderer_DX12.h"

namespace axe {

static Renderer* Renderer_instance = nullptr;

Renderer* Renderer::s_instance() { return Renderer_instance; }

Renderer_CreateDesc::Renderer_CreateDesc() noexcept
	: multithread(false)
{
#if AXE_OS_WINDOWS
	api = RendererApi::Dx12;
#else
	api = RendererApi::Vk;
#endif
}

Renderer* Renderer::s_create(const CreateDesc& desc) {
	Renderer* p = nullptr;

	switch (desc.api)
	{
		#if AXE_RENDER_HAS_DX12
			case RendererApi::Dx12: p = new Renderer_DX12(desc); break;
		#endif
		#if AXE_RENDER_HAS_VK
			case RendererApi::Vk:	p = new Renderer_Vk(desc); break;
		#endif
	//----
		default:
			AXE_THROW;
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

Renderer::Renderer(const CreateDesc& desc) noexcept
	: _multithread(desc.multithread)
{
	AXE_ASSERT(Renderer_instance == nullptr);
	Renderer_instance = this;
}

Renderer::~Renderer() noexcept
{
	AXE_ASSERT(Renderer_instance == this);
	AXE_ASSERT(_devices.size() == 0);

	Renderer_instance = nullptr;
}

UPtr<RenderContext> Renderer::newRenderContext(const RenderContext_CreateDesc& desc, int deviceIndex) {
	auto* device = findDevice(deviceIndex);
	AXE_ASSERT(device != nullptr);
	return device->createContext(desc);
}

} // namespace axe