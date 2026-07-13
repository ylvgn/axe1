#include "Context_Vk_Base.h"

#if AXE_RENDER_HAS_VK

namespace axe {

Context_Vk_Base::Context_Vk_Base(RenderDevice& device, const CreateDesc& desc)
	: Base(device, desc)
{
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK