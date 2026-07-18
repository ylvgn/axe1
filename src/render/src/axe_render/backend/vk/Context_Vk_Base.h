#pragma once

#include <axe_render/RenderContext.h>
#include "Common_Vk.h"

#if AXE_RENDER_HAS_VK

namespace axe {

class Context_Vk_Base : public RenderContext {
	AXE_RTTI_INFO(Context_Vk_Base, RenderContext)
public:
	Context_Vk_Base(RenderDevice& device, const CreateDesc& desc);
	
	Device_Vk*	renderDevice();
	
	~Context_Vk_Base();
}; // Context_Vk_Base

} // namespace axe

#endif // AXE_RENDER_HAS_VK