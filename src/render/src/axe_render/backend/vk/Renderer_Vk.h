#pragma once

#if AXE_RENDER_HAS_VK

#include <axe_render/Renderer.h>
#include "AXE_Vulkan.h"

namespace axe {

class Renderer_Vk : public Renderer {
	AXE_RTTI_INFO(Renderer_Vk, Renderer)
public:
	AXE_DOWNCAST_GET_INSTANCE()

	Renderer_Vk(const CreateDesc& desc);
	
	Device_Vk*		 findDevice(int i) const;
	Span<Device_Vk*> devices();
	::VkDevice		 vkDevice(int i);
	
	AXE_VkPhysicalDeviceList& physicalDeviceList()	{ return _gpuList; }
	AXE_VkInstance&			  vkInst()				{ return _vkInst; }
protected:
	virtual RenderDevice* onCreateRenderDevice(RenderDevice_CreateDesc& desc) final;
	AXE_RenderObject_LIST(AXE_RenderSystem_NewObject, Vk, override)
private:
	void _createVkInstance();
	void _getHardwareAdapterBasicInfo();
	
	AXE_VkInstance				_vkInst;
	AXE_VkPhysicalDeviceList	_gpuList;
}; // Renderer_Vk

} // namespace axe

#endif // AXE_TRY_USE_VK
