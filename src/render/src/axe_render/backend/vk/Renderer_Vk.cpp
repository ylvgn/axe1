#if AXE_RENDER_HAS_VK

#include "Renderer_Vk.h"
#include "Device_Vk.h"

namespace axe {

AXE_RenderObject_LIST(AXE_RenderSystem_NewObjectImp, Vk, override)

Renderer_Vk::Renderer_Vk(const CreateDesc& desc)
	: Base(desc)
{
	_createVkInstance();
	_getHardwareAdapterBasicInfo();
}

Device_Vk* Renderer_Vk::findDevice(int i) const {
	return static_cast<Device_Vk*>(Base::findDevice(i));
}

void Renderer_Vk::_createVkInstance() {
	::VkResult err;

	err = volkInitialize();
	VkUtil::throwIfError(err);
	
	AXE_VkInstanceCreateInfo info;
#if AXE_RENDER_DEBUG_LAYER
	info.logInfo();
#endif
	
	{ // extensions
		info.enableExtension(VK_KHR_SURFACE_EXTENSION_NAME);
		info.tryEnableExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		info.tryEnableExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	#if AXE_RENDER_DEBUG_LAYER
		info.enableExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

	#if AXE_OS_ANDROID
		info.enableExtension(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
	#elif AXE_OS_WINDOWS
		info.enableExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
	#elif AXE_OS_MACOSX
		info.enableExtension(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
		info.enableExtension(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
	#else
		#error "unsupported platform"
	#endif
	}
	
	{ // layers
	#if AXE_RENDER_DEBUG_LAYER
		info.tryEnableLayer("VK_LAYER_KHRONOS_validation");
	#endif
	}
	
	_vkInst.create(info);
	volkLoadInstance(_vkInst.handle());
	
	_gpuList.create(_vkInst);
}

void Renderer_Vk::_getHardwareAdapterBasicInfo() {
	for (auto& phyDev : _gpuList.devices()) {
		auto& props = phyDev.props();
		const auto& baseProps = props.base.properties;
		
		auto& adapterInfo = _adapterInfos.emplace_back();
		
		const auto& apiVersion = baseProps.apiVersion;
		u32 major = VkUtil::makeApiVersionMajor(apiVersion);
		u32 minor = VkUtil::makeApiVersionMinor(apiVersion);
		u32 patch = VkUtil::makeApiVersionPatch(apiVersion);
		
		UtfUtil::convert(adapterInfo.name, baseProps.deviceName);
		
		String deviceUUID;
		VkUtil::convert(deviceUUID, props.ID.deviceUUID);
		
		String driverUUID;
		VkUtil::convert(driverUUID, props.ID.driverUUID);
		
		String deviceLUID;
		if (props.ID.deviceLUIDValid == VK_TRUE) {
			VkUtil::convert(deviceLUID, props.ID.deviceLUID);
		}

		VkPhysicalDeviceMemoryProperties memProps;
		::vkGetPhysicalDeviceMemoryProperties(phyDev, &memProps);

		VkDeviceSize dedicatedVideoMemory = 0;
		VkDeviceSize sharedSystemMemory   = 0;
		for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i) {
			if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
				dedicatedVideoMemory += memProps.memoryHeaps[i].size;
			} else {
				// Heaps without the 'DEVICE_LOCAL' flag are generally host-accessible system memory.
				sharedSystemMemory += memProps.memoryHeaps[i].size;
			}
		}
		axe_try_safe_assign(adapterInfo.memorySize, dedicatedVideoMemory);
		
		AXE_LOG("Vulkan Physical Device = {}\n"
				"    ApiVersion = {}.{}.{}({})\n"
				"    DriverVersion = {}\n"
				"    VendorId = {}\n"
				"    DeviceId = {}\n"
				"    DeviceType = {}\n"
				"    deviceUUID = {}\n"
				"    driverUUID = {}\n"
				"    deviceLUID = {}\n"
				"    Video  Memory = {}MB\n"
				"    Shared Memory = {}MB\n"
//				"    ===== Monitor Info =====" TODO
			  , adapterInfo.name
			  , major, minor, patch, apiVersion
			  , baseProps.driverVersion
			  , baseProps.vendorID
			  , baseProps.deviceID
			  , baseProps.deviceType
			  , deviceUUID
			  , driverUUID
			  , deviceLUID
			  , Math::byteToM(dedicatedVideoMemory)
			  , Math::byteToM(sharedSystemMemory)
		);

#if 0 // TODO Monitor Info, no need atm
		uint32_t displayCount {0};
		::vkGetPhysicalDeviceDisplayPropertiesKHR(phyDev, &displayCount, nullptr);
		Vector<VkDisplayPropertiesKHR> displayProperties;
		displayProperties.resize(displayCount);
		::vkGetPhysicalDeviceDisplayPropertiesKHR(phyDev, &displayCount, displayProperties.data());
		
		int outputIndex = 0;
		for (const auto& displayProps : displayProperties) {
			TempString displayName;
			UtfUtil::convert(displayName, displayProps.displayName);
			
			AXE_LOG("    Monitor {}({}) - {}x{}\n"
					"        PhyDimensions: {}x{}\n"
			  , displayName, outputIndex
			  , displayProps.physicalResolution.width, displayProps.physicalResolution.height
			  , displayProps.physicalDimensions.width, displayProps.physicalDimensions.height);
			
			outputIndex++;
		}
#endif
	}
}

Span< Device_Vk* > Renderer_Vk::devices() {
	using DST = Device_Vk*;
	return Span<DST>(reinterpret_cast<DST*>(_devices.data()), _devices.size());
}

::VkDevice Renderer_Vk::vkDevice(int i) {
	auto* p = findDevice(i);
	return p ? p->vkDevice().handle() : VK_NULL_HANDLE;
}

RenderDevice* Renderer_Vk::onCreateRenderDevice(RenderDevice_CreateDesc& desc) {
	return new Device_Vk(desc);
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12
