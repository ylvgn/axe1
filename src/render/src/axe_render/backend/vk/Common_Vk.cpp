#if AXE_RENDER_HAS_VK

#include "Common_Vk.h"
#include "Renderer_Vk.h"
#include "Device_Vk.h"

namespace axe {

AXE_VkAllocatorCallbacks* VkUtil::allocCallbacks() {
#if 0
	static AXE_VkAllocatorCallbacks s;
	return &s;
#else
	return nullptr;
#endif
}

void AXE_VkBaseIOStructure::_setNext(VkBaseOutStructure* p) {
	_s_setNextImpl(_out, p);
}
void AXE_VkBaseIOStructure::_setNext(VkBaseInStructure* p) {
	_s_setNextImpl(_in, p);
}

Renderer_Vk* VkUtil::renderer() {
	return Renderer_Vk::s_instance();
}

Device_Vk* VkUtil::rootDevice() {
	return static_cast<Device_Vk*>(Renderer::s_rootDevice());
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK

