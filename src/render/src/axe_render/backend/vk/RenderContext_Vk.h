#pragma once

#if AXE_RENDER_HAS_VK

#if AXE_OS_WINDOWS
	#include "Context_Vk_Win32.h"

	namespace axe {
		using RenderContext_Vk_Impl = Context_Vk_Win32;
	}

#else
	#error "unsupported platform"
#endif


namespace axe {

class RenderContext_Vk : public RenderContext_Vk_Impl {
	AXE_RTTI_INFO(RenderContext_Vk, RenderContext_Vk_Impl)
public:
	RenderContext_Vk(RenderDevice& device, const CreateDesc& desc)
		: Base(device, desc)
	{}

}; // RenderContext_Vk

} // namespace axe

#endif // AXE_RENDER_HAS_VK