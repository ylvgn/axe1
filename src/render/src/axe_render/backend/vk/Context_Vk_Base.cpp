#include "Context_Vk_Base.h"
#include "Device_Vk.h"

#if AXE_RENDER_HAS_VK

namespace axe {

Context_Vk_Base::Context_Vk_Base(RenderDevice& device, const CreateDesc& desc)
	: Base(device, desc)
{
}

Device_Vk* Context_Vk_Base::renderDevice() {
	return static_cast<Device_Vk*>(_device);
}

Context_Vk_Base::~Context_Vk_Base() {
	if (auto* d = renderDevice()) {
		d->vkDevice().waitIdle(); // wait all commandQueue is done
	}
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK