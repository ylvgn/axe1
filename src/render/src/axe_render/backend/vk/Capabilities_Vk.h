#pragma once

#if AXE_RENDER_HAS_VK

#include <axe_render/RenderCapabilities.h>
#include "Common_Vk.h"

namespace axe {

class Capabilities_Vk : public RenderCapabilities {
	AXE_RTTI_INFO(Capabilities_Vk, RenderCapabilities)
public:
	Capabilities_Vk(const VkPhysicalDevice& phyDev);

	struct Features : public AXE_VkBaseIOStructure {
		Features();
		::VkPhysicalDeviceFeatures2							base 				{};
		::VkPhysicalDeviceVulkan11Features					v11  				{};
		::VkPhysicalDeviceVulkan12Features					v12  				{};
		::VkPhysicalDeviceVulkan13Features					v13  				{};
		::VkPhysicalDeviceVulkan14Features					v14  				{};
		::VkPhysicalDeviceFragmentShadingRateFeaturesKHR	fragmentShadingRate {};
		::VkPhysicalDeviceMeshShaderFeaturesEXT				meshShader			{};
		::VkPhysicalDevicePresentTimingFeaturesEXT			presentTiming		{};
#ifdef VK_KHR_ray_tracing_pipeline
		::VkPhysicalDeviceRayTracingPipelineFeaturesKHR		rayTracingPipeline	{};
#endif
	};
	
	const Features& features() const { return _features; }
private:
	Features _features;
}; // Capabilities_Vk

} // namespace axe

#endif // AXE_RENDER_HAS_VK