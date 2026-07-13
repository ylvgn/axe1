#pragma once

#if AXE_RENDER_HAS_VK

#include <axe_render/RenderDevice.h>
#include "AXE_Vulkan.h"

namespace axe {

class Device_Vk : public RenderDevice {
	AXE_RTTI_INFO(Device_Vk, RenderDevice)
	using Util = VkUtil;
public:
	Device_Vk(CreateDesc& desc);

	operator ::VkDevice()					{ return _device; }
	operator ::VkPhysicalDevice()			{ return _phyDev ? _phyDev->handle() : VK_NULL_HANDLE; }
	operator ::VkInstance()					{ return vkInst(); }
	
	::VkInstance			vkInst()			{ return _phyDev->inst(); }
	AXE_VkDevice&			vkDevice()			{ return _device; }
	AXE_VkPhysicalDevice*	physicalDevice()	{ return _phyDev; }

private:
	AXE_VkDevice			_device;
	AXE_VkPhysicalDevice*	_phyDev = nullptr;
}; // Device_Vk

} // namespace axe

#endif // AXE_RENDER_HAS_VK