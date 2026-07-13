#if AXE_RENDER_HAS_VK

#include "Capabilities_Vk.h"

namespace axe {

Capabilities_Vk::Features::Features() {
	base.sType       		  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	v11.sType        		  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	v12.sType        		  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	v13.sType        		  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	v14.sType        		  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
	fragmentShadingRate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
	meshShader.sType          = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
	presentTiming.sType       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PRESENT_TIMING_FEATURES_EXT;
#ifdef VK_KHR_ray_tracing_pipeline
	rayTracingPipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
#endif
	
#define AXE_MACRO_OP(E) \
	_setNext(reinterpret_cast<VkBaseOutStructure*>(&E)); \
//----
	AXE_MACRO_OP(base) // base MUST be first
	AXE_MACRO_OP(v11)
	AXE_MACRO_OP(v12)
	AXE_MACRO_OP(v13)
	AXE_MACRO_OP(v14)
	AXE_MACRO_OP(fragmentShadingRate)
	AXE_MACRO_OP(meshShader)
	AXE_MACRO_OP(presentTiming)
	#ifdef VK_KHR_ray_tracing_pipeline
		AXE_MACRO_OP(rayTracingPipeline)
	#endif
#undef AXE_MACRO_OP
}

Capabilities_Vk::Capabilities_Vk(const VkPhysicalDevice& phyDev) {
	vkGetPhysicalDeviceFeatures2(phyDev, &_features.base);
	
	_info.hasComputeShader	= true; // Always true; mandatory in Vulkan
	
	_info.shaderHasFloat64	= _features.base.features.shaderFloat64 == VK_TRUE;
#ifdef VK_KHR_ray_tracing_pipeline
	_info.hasRaytracing		= _features.rayTracingPipeline.rayTracingPipeline == VK_TRUE;
#endif
	_info.hasMeshShader		= _features.meshShader.meshShader == VK_TRUE;
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK