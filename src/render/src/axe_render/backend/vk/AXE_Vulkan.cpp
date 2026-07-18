#if AXE_RENDER_HAS_VK

#include "AXE_Vulkan.h"
#include "Device_Vk.h"
#include "Capabilities_Vk.h"

namespace axe {

AXE_VkApplicationInfo::AXE_VkApplicationInfo() {
	sType			 	= VK_STRUCTURE_TYPE_APPLICATION_INFO;
	pNext			 	= nullptr;
	pApplicationName 	= "Renderer_Vk";
	applicationVersion	= 0;
	pEngineName		 	= "Renderer_Vk";
	engineVersion		= 0;
	apiVersion			= VK_API_VERSION_1_4;
}

AXE_VkInstanceCreateInfo::AXE_VkInstanceCreateInfo() {
	sType 					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	pNext 					= nullptr;
	flags 					= 0;
	pApplicationInfo		= &appInfo;
	enabledLayerCount		= 0;
	ppEnabledLayerNames 	= nullptr;
	enabledExtensionCount	= 0;
	ppEnabledExtensionNames = nullptr;
}

AXE_VkInstanceCreateInfo::Props::Props() {
	::VkResult err;
	
	{
		uint32_t layerCount { 0 };
		err = ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		VkUtil::throwIfError(err);
		
		availableLayers.resize(layerCount);
		err = ::vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		VkUtil::throwIfError(err);
	}

	{
		uint32_t extensionCount { 0 };
		err = ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		VkUtil::throwIfError(err);
		
		availableExtensions.resize(extensionCount);
		err = ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
		VkUtil::throwIfError(err);
	}
}

bool AXE_VkInstanceCreateInfo::Props::hasLayer(StrView name) const {
	for (auto& e : availableLayers) {
		if (StrView_c_str(e.layerName) == name)
			return true;
	}
	return false;
}
bool AXE_VkInstanceCreateInfo::Props::hasExtension(StrView name) const {
	for (auto& e : availableExtensions) {
		if (StrView_c_str(e.extensionName) == name)
			return true;
	}
	return false;
}

void AXE_VkInstanceCreateInfo::enableLayer(StrLit name) {
	if (!tryEnableLayer(name)) {
		AXE_THROW;
	}
}
bool AXE_VkInstanceCreateInfo::tryEnableLayer(StrLit name) {
	if (props.hasLayer(name)) {
		_enabledLayerNames.push_back(name);
		return true;
	}
	return false;
}
	
void AXE_VkInstanceCreateInfo::enableExtension(StrLit name) {
	if (!tryEnableExtension(name)) {
		AXE_THROW;
	}
}
bool AXE_VkInstanceCreateInfo::tryEnableExtension(StrLit name) {
	if (props.hasExtension(name)) {
		_enabledExtensionNames.push_back(name);
		return true;
	}
	return false;
}

void AXE_VkInstanceCreateInfo::logInfo() {
	TempString s = "== AXE_VkInstanceCreateInfo::logInfo ==\n";
	s.append("VkInstance Layers\n");
	for (auto& e : props.availableLayers) {
		FmtTo(s, "  {:40} spec={}, impl={:3}, desc={}\n",
			StrView_c_str(e.layerName),
			e.specVersion,
			e.implementationVersion,
			StrView_c_str(e.description));
	}

	s.append("VkInstance Extensions\n");
	for (auto& e : props.availableExtensions) {
		FmtTo(s, "  {:40} spec={}\n",
			StrView_c_str(e.extensionName),
			e.specVersion);
	}
	AXE_LOG("{}", s);
}

VkBool32 AXE_VkInstance::onValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
											  VkDebugUtilsMessageTypeFlagsEXT messageType,
											  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
	
	TempString msg;
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
		FmtTo(msg, "[Validation] {}", pCallbackData->pMessage);
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			FmtTo(msg, "[Performance] {}", pCallbackData->pMessage);
		}
	}
	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			FmtTo(msg, "[General] {}", pCallbackData->pMessage);
		}
	}

	if (!msg.empty()) {
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
			AXE_LOG_VERB("{}", msg);
		} else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			AXE_LOG_INFO("{}", msg);
		} else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			AXE_LOG_WARN("{}", msg);
		} if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			AXE_LOG_ERROR("{}", msg);
			AXE_ASSERT(false);
		}
	}
	
	return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL AXE_VkInstance::s_debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
															   				 VkDebugUtilsMessageTypeFlagsEXT messageType,
															   				 const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
															   				 void* pUserData) {
	// enabled 'VK_LAYER_KHRONOS_validation' then log will sent here.
	if (auto* thisObj = reinterpret_cast<AXE_VkInstance*>(pUserData)) {
		return thisObj->onValidationCallback(messageSeverity, messageType, pCallbackData);
	}
	return VK_FALSE; // Must return VK_FALSE to continue execution
}

void AXE_VkInstance::create(AXE_VkInstanceCreateInfo& createInfo) {
	destroy();
	::VkResult err;

	err = ::vkEnumerateInstanceVersion(&createInfo.appInfo.apiVersion);
	VkUtil::throwIfError(err);
	
	createInfo.enabledLayerCount		= VkUtil::castUInt32(createInfo._enabledLayerNames.size());
	createInfo.ppEnabledLayerNames		= createInfo._enabledLayerNames.data();
	createInfo.enabledExtensionCount	= VkUtil::castUInt32(createInfo._enabledExtensionNames.size());
	createInfo.ppEnabledExtensionNames	= createInfo._enabledExtensionNames.data();

	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	err = ::vkCreateInstance(&createInfo, pAllocator, &_instance);
	VkUtil::throwIfError(err);

	{
		::VkDebugUtilsMessengerCreateInfoEXT ci {};
		ci.sType							= VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		ci.messageSeverity  				= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		ci.messageType						= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		ci.pfnUserCallback  				= s_debugUtilsMessengerCallback;
		ci.pUserData						= this;
#if	AXE_RENDER_DEBUG_LAYER
		ci.messageSeverity = ci.messageSeverity
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		
		ci.messageType = ci.messageType
			| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
#endif

		auto pfnCreateWin32SurfaceKHR = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
		if (pfnCreateWin32SurfaceKHR != nullptr) {
			err = pfnCreateWin32SurfaceKHR(_instance, &ci, nullptr, &_debugMessenger);
			VkUtil::throwIfError(err);
		}
	}
}

void AXE_VkInstance::destroy() {
	if (_debugMessenger) {
		vkDestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
		_debugMessenger = VK_NULL_HANDLE;
	}
	if (_instance) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		vkDestroyInstance(_instance, pAllocator);
		_instance = VK_NULL_HANDLE;
	}
}

AXE_VkPhysicalDevice::Properties::Properties() {
	base.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	ID.sType   = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
	
#define AXE_MACRO_OP(E) \
	_setNext(reinterpret_cast<VkBaseOutStructure*>(&E)); \
//----
	AXE_MACRO_OP(base) // base MUST be first
	AXE_MACRO_OP(ID)
#undef AXE_MACRO_OP
}

AXE_VkPhysicalDevice::AXE_VkPhysicalDevice(AXE_VkPhysicalDevice && r) noexcept
	: _props(AXE_MOVE(r._props))
	, _queueFamilyProps(AXE_MOVE(r._queueFamilyProps))
	, _index(AXE_MOVE(r._index))
	, _phyDev(AXE_MOVE(r._phyDev))
	, _inst(AXE_MOVE(r._inst))
{
	r._index	= -1;
	r._phyDev	= VK_NULL_HANDLE;
	r._inst		= VK_NULL_HANDLE;
	r._props = {};
}

void AXE_VkPhysicalDevice::create(Int index, VkInstance inst, VkPhysicalDevice phyDev) {
	::VkResult err;
	
	_index  = index;
	_phyDev = phyDev;
	_inst   = inst;
	
	::vkGetPhysicalDeviceProperties2(_phyDev, &_props.base);
	
	{
		uint32_t queueFamilyCount {0};
		::vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, nullptr);
		AXE_ASSERT(queueFamilyCount >= 1);
		
		_queueFamilyProps.resize(queueFamilyCount);
		::vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, _queueFamilyProps.data());
		AXE_ASSERT(queueFamilyCount >= 1);
	}
	
	{
		uint32_t extensionCount {0};
		err = vkEnumerateDeviceExtensionProperties(_phyDev, nullptr, &extensionCount, nullptr);
		VkUtil::throwIfError(err);

		_availableExtensions.resize(extensionCount);
		err = vkEnumerateDeviceExtensionProperties(_phyDev, nullptr, &extensionCount, _availableExtensions.data());
		VkUtil::throwIfError(err);
	}
}

bool AXE_VkPhysicalDevice::hasExtension(StrView name) const {
	for (auto& e : _availableExtensions) {
		if (e.extensionName == name) return true;
	}
	return false;
}

Opt<AXE_VkQueueFamilyIndex> AXE_VkPhysicalDevice::findQueueFamilyIndex(::VkQueueFlags hasFlags) const {
	using EnumIntT = Type_EnumInt<AXE_VkQueueFamilyIndex>;
	EnumIntT i = 0;
	for (const auto& p : _queueFamilyProps) {
		if (p.queueFlags & hasFlags) {
			return static_cast<AXE_VkQueueFamilyIndex>(i);
		}
		i++;
	}
	return ::eastl::nullopt;
}

Opt<AXE_VkQueueFamilyIndex> AXE_VkPhysicalDevice::findGraphicFamilyIndex() const {
	return findQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
}
Opt<AXE_VkQueueFamilyIndex> AXE_VkPhysicalDevice::findComputeQueueFamilyIndex() const {
	using EnumIntT = Type_EnumInt<AXE_VkQueueFamilyIndex>;
	EnumIntT i = 0;
	AXE_VkQueueFamilyIndex graphQueueCanCompute = AXE_VkQueueFamilyIndex::Invalid;

	for (auto& p : _queueFamilyProps) {
		if (p.queueFlags & VK_QUEUE_COMPUTE_BIT) {

			if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphQueueCanCompute = static_cast<AXE_VkQueueFamilyIndex>(i);
			} else {
				return static_cast<AXE_VkQueueFamilyIndex>(i);
			}
		}
		i++;
	}

	if (graphQueueCanCompute != AXE_VkQueueFamilyIndex::Invalid) {
		return graphQueueCanCompute;
	}
	return ::eastl::nullopt;
}

void AXE_VkPhysicalDeviceList::create(VkInstance inst) {
	::VkResult err;
	Vector<::VkPhysicalDevice, 8> tmpPhyDevices;

	{
		uint32_t gpuCount{ 0 };
		err = ::vkEnumeratePhysicalDevices(inst, &gpuCount, nullptr);
		VkUtil::throwIfError(err);

		tmpPhyDevices.resize(gpuCount);
		err = ::vkEnumeratePhysicalDevices(inst, &gpuCount, tmpPhyDevices.data());
		VkUtil::throwIfError(err);
		
		AXE_ASSERT(tmpPhyDevices.size() == gpuCount);
	}
	
	_devices.resize(tmpPhyDevices.size());
	
	Int i = 0;
	for (auto& phyDev : tmpPhyDevices) {
		_devices[i].create(i, inst, phyDev);
		i++;
	}
}

void AXE_VkDevice::destroy() {
	if (_vmaAllocator) {
		vmaDestroyAllocator(_vmaAllocator);
		_vmaAllocator = VK_NULL_HANDLE;
	}
	
	if (_handle) {
		::vkDeviceWaitIdle(_handle);
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		::vkDestroyDevice(_handle, pAllocator);
		_handle = VK_NULL_HANDLE;
	}
}

AXE_VkPhysicalDevice* AXE_VkDevice::physicalDevice() { return _device->physicalDevice(); }
AXE_VkDevice::operator ::VkPhysicalDevice()	{ return physicalDevice()->handle(); }
	
::VkInstance AXE_VkDevice::inst()	  { return _device->vkInst(); }
AXE_VkDevice::operator ::VkInstance() { return inst(); }

void AXE_VkDevice::create(Device_Vk& device) {
	::VkResult err;
	
	destroy();
	_device = &device;
	
	auto* phyDev = _device->physicalDevice();
	AXE_ASSERT(phyDev != nullptr);
	
	auto* capabilities = static_cast<Capabilities_Vk*>(_device->capabilities());
	AXE_ASSERT(capabilities != nullptr);
	
	Vector<const char*, 16> tmpEnabledExtensionNames;
	
	auto tryEnableExtension = [&](StrLit name) -> bool {
		if (!phyDev->hasExtension(name)) {
			AXE_LOG("AXE_VkDevice: Cannot enable VkDevice extension {}", name);
			return false;
		}
		tmpEnabledExtensionNames.push_back(name);
		return true;
	};
	
	auto enableExtension = [&](StrLit name) -> void {
		if (!tryEnableExtension(name)) {
			throw Error(Fmt("AXE_VkDevice: Cannot enable VkDevice extension {}", ::axe::StrView_c_str(name)), AXE_LOC);
		}
	};
	enableExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	
	if constexpr(true) { // VMA allocator support extensions
		tryEnableExtension(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
		tryEnableExtension(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
		tryEnableExtension(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
		tryEnableExtension(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
		tryEnableExtension(VK_EXT_MEMORY_BUDGET_EXTENSION_NAME);
		tryEnableExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
		tryEnableExtension(VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME);
		tryEnableExtension(VK_AMD_DEVICE_COHERENT_MEMORY_EXTENSION_NAME);
		tryEnableExtension(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
	}
	
	if constexpr(true) { // Mesh Shader
		tryEnableExtension(VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME);
		tryEnableExtension(VK_EXT_MESH_SHADER_EXTENSION_NAME);
	}
	
// optional extensions
	tryEnableExtension(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
	// already included in Vulkan 1.1
	tryEnableExtension(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
	tryEnableExtension(VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME);
//------
	
	if (auto opt = phyDev->findGraphicFamilyIndex()) {
		_graphQueueFamilyIndex = opt.value();
	} else { 
		AXE_THROW;
	}
	
	const float queuePriorities { 1.0f };
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext						= nullptr;
	queueCreateInfo.queueCount					= 1;
	queueCreateInfo.pQueuePriorities			= &queuePriorities;
	queueCreateInfo.queueFamilyIndex			= enumInt(_graphQueueFamilyIndex);
	
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext						= &capabilities->features().base;
	deviceCreateInfo.flags						= 0;
	deviceCreateInfo.queueCreateInfoCount		= 1;
	deviceCreateInfo.pQueueCreateInfos			= &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount			= 0;
	deviceCreateInfo.ppEnabledLayerNames		= nullptr;
	deviceCreateInfo.enabledExtensionCount		= VkUtil::castUInt32(tmpEnabledExtensionNames.size());
	deviceCreateInfo.ppEnabledExtensionNames	= tmpEnabledExtensionNames.data();
	deviceCreateInfo.pEnabledFeatures			= nullptr; // must be null when pNext = VkPhysicalDeviceFeatures2

	err = vkCreateDevice(*phyDev, &deviceCreateInfo, nullptr, &_handle);
	VkUtil::throwIfError(err);
	volkLoadDevice(_handle);

	_createAllocator();
}

void AXE_VkDevice::_createAllocator() {
	::VkResult err;
	
	VmaVulkanFunctions vkFunctions {};
	vkFunctions.vkGetPhysicalDeviceProperties 			= vkGetPhysicalDeviceProperties;
	vkFunctions.vkGetPhysicalDeviceMemoryProperties 	= vkGetPhysicalDeviceMemoryProperties;
	vkFunctions.vkAllocateMemory 						= vkAllocateMemory;
	vkFunctions.vkFreeMemory 							= vkFreeMemory;
	vkFunctions.vkMapMemory 							= vkMapMemory;
	vkFunctions.vkUnmapMemory 							= vkUnmapMemory;
	vkFunctions.vkFlushMappedMemoryRanges 				= vkFlushMappedMemoryRanges;
	vkFunctions.vkInvalidateMappedMemoryRanges 			= vkInvalidateMappedMemoryRanges;
	vkFunctions.vkBindBufferMemory 						= vkBindBufferMemory;
	vkFunctions.vkBindImageMemory 						= vkBindImageMemory;
	vkFunctions.vkGetBufferMemoryRequirements 			= vkGetBufferMemoryRequirements;
	vkFunctions.vkGetImageMemoryRequirements 			= vkGetImageMemoryRequirements;
	vkFunctions.vkCreateBuffer 							= vkCreateBuffer;
	vkFunctions.vkDestroyBuffer 						= vkDestroyBuffer;
	vkFunctions.vkCreateImage 							= vkCreateImage; 
	vkFunctions.vkDestroyImage 							= vkDestroyImage;
	vkFunctions.vkCmdCopyBuffer 						= vkCmdCopyBuffer;
	vkFunctions.vkGetBufferMemoryRequirements2KHR		= vkGetBufferMemoryRequirements2;
	vkFunctions.vkGetImageMemoryRequirements2KHR		= vkGetImageMemoryRequirements2;

	VmaAllocatorCreateInfo ci {};
	ci.flags				= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	ci.vulkanApiVersion    	= VK_API_VERSION_1_4;
	ci.physicalDevice	    = physicalDevice()->handle();
	ci.device			    = _handle;
	ci.pVulkanFunctions		= &vkFunctions;
	ci.instance				= inst();
	ci.pAllocationCallbacks	= nullptr;
	
	err = vmaCreateAllocator(&ci, &_vmaAllocator);
	VkUtil::throwIfError(err);
}

void AXE_VkDevice::getQueue(class AXE_VkDeviceQueue& outQueue, AXE_VkQueueFamilyIndex queueFamilyIndex, u32 queueIndex) {
	outQueue._dev = this;
	vkGetDeviceQueue(*this, enumInt(queueFamilyIndex), queueIndex, &outQueue._handle);
}

void AXE_VkDeviceQueue::submit(Span<::VkSubmitInfo> infos, VkFence fenceToSignal) {
	if (infos.empty()) return;
	/*
	 * vkQueueSubmit is telling the GPU where to find the command buffer in CPU memory. 
	 * no uploading commands to GPU, The GPU reads your command buffer directly from RAM while it's executing.
	 */
	auto err = ::vkQueueSubmit(_handle
									  , static_cast<uint32_t>(infos.size())
									  , infos.data()
									  , fenceToSignal);
	/*
	* This returns IMMEDIATELY, often before the GPU even starts working!
	* The GPU will process this command buffer later, so command buffer is on "Pending" state
	* until 'vkGetFenceStatus(device, fence) == VK_SUCCESS' then GPU finished command buffer. after that, command buffer could safe call vkResetCommandBuffer
	*/
	
	if (!VkUtil::checkResult(err)) {
		uint32_t count = 0;
//		renderSystem_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count, nullptr);
		Vector<VkCheckpointDataNV, 128> checkPoints;
		checkPoints.resize(count);

		for (auto& e : checkPoints) {
			e.sType				= VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;
			e.pNext				= nullptr;
			e.pCheckpointMarker = nullptr;
			e.stage				= VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;
		}

//		renderSystem_->procList()->vkGetQueueCheckpointDataNV(_swapChainData->_graphQueue, &count, checkPoints.data());

		for (auto& e : checkPoints.revForEach()) { // for (auto it = checkPoints.rbegin(); it != checkPoints.rend(); ++it)
			// auto& e = *it; 
			AXE_LOG("checkPoint {}", StrView_c_str(static_cast<const char*>(e.pCheckpointMarker)));
		}
	}
	VkUtil::throwIfError(err);
}

void AXE_VkSurfaceKHR::destroy() {
	if (_surface) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		vkDestroySurfaceKHR(_dev->inst(), _surface, pAllocator);
		_surface = VK_NULL_HANDLE;
		_dev = nullptr;

		_availableFormats.clear();
		_availablePresentModes.clear();

//		_graphQueueFamilyIndex   = AXE_VkQueueFamilyIndex::Invalid;
//		_presentQueueFamilyIndex = AXE_VkQueueFamilyIndex::Invalid;
	}
}

#if AXE_OS_WINDOWS
void AXE_VkSurfaceKHR::create_Win32(AXE_VkDevice& dev, ::HINSTANCE hInstance, ::HWND hwnd) {
	destroy();
	_dev = &dev;

	::VkWin32SurfaceCreateInfoKHR ci {};
	ci.sType 	 					 = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	ci.pNext 	 					 = nullptr;
	ci.flags 	 					 = 0;
	ci.hinstance 					 = hInstance;
	ci.hwnd		 					 = hwnd;
	
	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = vkCreateWin32SurfaceKHR(_dev->inst(), &ci, pAllocator, &_surface);
	VkUtil::throwIfError(err);

	_onCreate();
}
#endif

void AXE_VkSurfaceKHR::_onCreate() {
	VkResult err;
	auto phyDev = _dev->physicalDevice();
	
	{
		uint32_t surfaceFormatCount {0};
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*phyDev, _surface, &surfaceFormatCount, nullptr);
		VkUtil::throwIfError(err);

		_availableFormats.resize(surfaceFormatCount);
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(*phyDev, _surface, &surfaceFormatCount, _availableFormats.data());
		VkUtil::throwIfError(err);
	}
	
	{
		uint32_t presentModeCount {0};
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*phyDev, _surface, &presentModeCount, nullptr);
		VkUtil::throwIfError(err);

		_availablePresentModes.resize(presentModeCount);
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(*phyDev, _surface, &presentModeCount, _availablePresentModes.data());
		VkUtil::throwIfError(err);
	}
#if 0 // no use atm
	{
		_graphQueueFamilyIndex = _dev->graphQueueFamilyIndex();

		_presentQueueFamilyIndex = AXE_VkQueueFamilyIndex::Invalid;
		if (checkQueueFamilySupportPresent(_graphQueueFamilyIndex)) {
			_presentQueueFamilyIndex = _graphQueueFamilyIndex; // graphQueue 'VK_QUEUE_GRAPHICS_BIT' can do preset too
		} else if (auto opt = findQueueFamilySupportPresent()) {
			_presentQueueFamilyIndex = opt.value();
		} else {
			AXE_THROW;
		}
	}
#else
	AXE_ASSERT(checkQueueFamilySupportPresent(_dev->graphQueueFamilyIndex()));
#endif
}

bool AXE_VkSurfaceKHR::checkQueueFamilySupportPresent(AXE_VkQueueFamilyIndex queueFamilyIndex) const {
	// physical device supports presentation to a given surface
	VkBool32 bSupportsPresent = VK_FALSE;
	auto err = vkGetPhysicalDeviceSurfaceSupportKHR(*_dev, enumInt(queueFamilyIndex), _surface, &bSupportsPresent);
	VkUtil::throwIfError(err);
	return VK_FALSE != bSupportsPresent;
}

Opt<AXE_VkQueueFamilyIndex> AXE_VkSurfaceKHR::findQueueFamilySupportPresent() const {
	using EnumIntT = Type_EnumInt<AXE_VkQueueFamilyIndex>;

	const auto& queueFamilyProps = _dev->physicalDevice()->queueFamilyProps();
	EnumIntT n = axe_safe_cast_from(queueFamilyProps.size());

	for (EnumIntT i = 0; i < n; ++i) {
		auto index = static_cast<AXE_VkQueueFamilyIndex>(i);
		if (checkQueueFamilySupportPresent(index)) {
			return index;
		}
	}
	return ::eastl::nullopt;
}

VkSurfaceCapabilitiesKHR AXE_VkSurfaceKHR::getCapabilities() {
	::VkSurfaceCapabilitiesKHR o {};
	auto err = ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*_dev, _surface, &o);
	VkUtil::throwIfError(err);
	return o;
}

bool AXE_VkSurfaceKHR::isSupportFormat(VkFormat format, VkColorSpaceKHR colorSpace) {
	for (auto& e : _availableFormats) {
		if (e.format == format && e.colorSpace == colorSpace) {
			return true;
		}
	}
	return false;
}

void AXE_VkSwapchainKHR::destroy() {
	if (_handle) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		vkDestroySwapchainKHR(*_dev, _handle, pAllocator);
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void AXE_VkSwapchainKHR::create(AXE_VkDevice&				dev,
								AXE_VkSurfaceKHR&			surface,
								const ::VkSurfaceFormatKHR&	format,
								Int							imageCount_,
								::VkPresentModeKHR			presentMode,
								AXE_VkSwapchainKHR*			oldSwapchain /*= nullptr*/) 
{
	destroy();
	_dev = &dev;
	
	if (!surface) AXE_THROW;
	if (!surface.isSupportFormat(format)) AXE_THROW;
	
	auto imageCount = VkUtil::castUInt32(imageCount_);

	auto cap = surface.getCapabilities();
	if (imageCount < cap.minImageCount) AXE_THROW;
	if (imageCount > cap.maxImageCount) AXE_THROW;
	
	::VkExtent2D swapchainExtent{ cap.currentExtent };
	if (cap.currentExtent.width == 0xFFFFFFFF) {
		swapchainExtent.width  = 8;
		swapchainExtent.height = 8;
	}
	
	Vector<u32, 2> queueFamilyIndices;
	queueFamilyIndices.emplace_back(enumInt(dev.graphQueueFamilyIndex()));
#if 0 // no need atm
	if (surface.isPresentQueueIsSeparated()) {
		queueFamilyIndices.emplace_back(enumInt(surface.presentQueueFamilyIndex()));
	}
#endif
	VkResult err;
	
	::VkSwapchainCreateInfoKHR ci {};
	ci.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	ci.pNext					= nullptr;
	ci.flags					= 0;
	ci.surface					= surface;
	ci.minImageCount			= cap.minImageCount;
	ci.imageFormat				= format.format;
	ci.imageColorSpace 			= format.colorSpace;
	ci.imageExtent				= swapchainExtent;
	ci.imageArrayLayers 		= 1;
	ci.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	ci.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	ci.queueFamilyIndexCount	= VkUtil::castUInt32(queueFamilyIndices.size());
	ci.pQueueFamilyIndices		= queueFamilyIndices.data();
	ci.preTransform				= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	ci.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	ci.presentMode				= presentMode;
	ci.clipped					= VK_TRUE;
	ci.oldSwapchain				= oldSwapchain ? oldSwapchain->handle() : nullptr;
	
	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	err = vkCreateSwapchainKHR(*_dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VkSwapchainKHR::getImages(Vector<VkImage>& outImages) {
	outImages.clear();
	
	::VkResult err;
	uint32_t imageCount {0};

	err = ::vkGetSwapchainImagesKHR(*_dev, _handle, &imageCount, nullptr);
	VkUtil::throwIfError(err);

	outImages.resize(imageCount);
	err = ::vkGetSwapchainImagesKHR(*_dev, _handle, &imageCount, outImages.data());
	VkUtil::throwIfError(err);
}

VkResult AXE_VkSwapchainKHR::acquireNextImage(u32& outImageIndex
											, VkSemaphore semaphoreToSignal
											, VkFence fenceToSignal
											, u64 timeout_u64)
{
	return ::vkAcquireNextImageKHR(*_dev, _handle, timeout_u64, semaphoreToSignal, fenceToSignal, &outImageIndex);
}

void AXE_VkCommandPool::destroy() {
	if (_handle) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		::vkDestroyCommandPool(*_dev, _handle, pAllocator);
		_handle = VK_NULL_HANDLE;
		_dev	= nullptr;
	}
}

void AXE_VkCommandPool::create(AXE_VkDevice& dev, AXE_VkQueueFamilyIndex queueFamilyIndex) {
	destroy();
	_dev = &dev;

	::VkCommandPoolCreateInfo ci = {};
	ci.sType 				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	ci.pNext 				= nullptr;
	ci.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	ci.queueFamilyIndex		= enumInt(queueFamilyIndex);

	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = ::vkCreateCommandPool(dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VkCommandBuffer::destroy() {
	if (_handle) {
		::vkFreeCommandBuffers(*_cmdPool->device(), *_cmdPool, 1, &_handle);
		_handle  = VK_NULL_HANDLE;
		_cmdPool = nullptr;
	}
}

AXE_VkCommandBuffer::AXE_VkCommandBuffer(AXE_VkCommandBuffer && r) noexcept
	: _handle(AXE_MOVE(r._handle))
	, _cmdPool(AXE_MOVE(r._cmdPool)) 
{
	r._handle  = VK_NULL_HANDLE;
	r._cmdPool = nullptr;
}

void AXE_VkCommandBuffer::create(AXE_VkCommandPool& cmdPool) {
	AXE_ASSERT(cmdPool.handle() != VK_NULL_HANDLE);
	destroy();
	_cmdPool = &cmdPool;

	::VkCommandBufferAllocateInfo ci = {};
	ci.sType 						 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	ci.pNext 						 = nullptr;
	ci.commandPool					 = cmdPool;
	ci.level						 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	ci.commandBufferCount			 = 1;

	auto err = ::vkAllocateCommandBuffers(*cmdPool.device(), &ci, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VkCommandBuffer::resetAndReleaseResource() {
	::vkResetCommandBuffer(_handle, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

void AXE_VkCommandBuffer::beginCommand() {
	::VkCommandBufferBeginInfo ci = {};
	ci.sType 					  = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	ci.pNext 					  = nullptr;
	ci.flags 					  = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // for Individual call vkResetCommandBuffer
	ci.pInheritanceInfo			  = nullptr;

	::vkBeginCommandBuffer(_handle, &ci);
}

void AXE_VkCommandBuffer::endCommand() {
	::vkEndCommandBuffer(_handle);
}

void AXE_VkImageView::destroy() {
	if (_handle) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		::vkDestroyImageView(*_dev, _handle, pAllocator);
		_handle = VK_NULL_HANDLE;
		_dev	= nullptr;
	}
}

void AXE_VkImageView::create(AXE_VkDevice& dev, VkImage image, VkFormat format) {
	::VkImageViewCreateInfo ci {};
	ci.sType 				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ci.image 				= image;
	ci.viewType 			= VK_IMAGE_VIEW_TYPE_2D;
	ci.format				= format;

	ci.subresourceRange  = {0};
	{
		auto& aspectMask = ci.subresourceRange.aspectMask;
		if (VkUtil::formatHasDepth(format))   aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (VkUtil::formatHasStencil(format)) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		if (!aspectMask) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	ci.subresourceRange.baseMipLevel	= 0;
	ci.subresourceRange.levelCount		= 1;
	ci.subresourceRange.baseArrayLayer	= 0;
	ci.subresourceRange.layerCount		= 1;
	
	create(dev, ci);
}

void AXE_VkImageView::create(AXE_VkDevice& dev, VkImageViewCreateInfo& ci) {
	destroy();
	
	_dev	= &dev;
	_format = ci.format;

	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = ::vkCreateImageView(dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VkImage::destroy() {
	if (_handle) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		vkDestroyImage(*_dev, _handle, pAllocator);
		_handle 		= VK_NULL_HANDLE;
		_dev			= nullptr;
	}
}

VkImageCreateInfo_Color::VkImageCreateInfo_Color(AXE_VkDevice& dev
											   , const Vec2i& frameSize
											   , ColorType colorType
											   , Int mipLevels_
) {
	sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	pNext				= nullptr;
	imageType			= VK_IMAGE_TYPE_2D;
	format				= VkUtil::getVkColorType(colorType);
	extent.width		= VkUtil::castUInt32(frameSize.x);
	extent.height		= VkUtil::castUInt32(frameSize.y);
	extent.depth		= 1;
	mipLevels			= VkUtil::castUInt32(mipLevels_);
	arrayLayers			= 1;
	samples				= VK_SAMPLE_COUNT_1_BIT;
	tiling				= VK_IMAGE_TILING_OPTIMAL;
	usage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	flags				= 0;
	initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageCreateInfo_DepthStencil::VkImageCreateInfo_DepthStencil(AXE_VkDevice& dev
															 , const Vec2i& frameSize
															 , RenderDepthType depthType
) {
	sType				= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	pNext				= nullptr;
	imageType			= VK_IMAGE_TYPE_2D;
	format				= VkUtil::getVkDepthType(depthType);
	extent.width		= VkUtil::castUInt32(frameSize.x);
	extent.height		= VkUtil::castUInt32(frameSize.y);
	extent.depth		= 1;
	mipLevels			= 1;
	arrayLayers			= 1;
	samples				= VK_SAMPLE_COUNT_1_BIT;
	tiling				= VK_IMAGE_TILING_OPTIMAL;
	usage				= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	flags				= 0;
	initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
}

void AXE_VkImage::createImage2D(AXE_VkDevice& dev, const Vec2i& frameSize, ColorType colorType, Int mipLevels) {
	ColorCI ci(dev, frameSize, colorType, mipLevels);
	_create(dev, ci);
}
void AXE_VkImage::createDepthStencil(AXE_VkDevice& dev, const Vec2i& frameSize, RenderDepthType depthType) {
	DepthStencilCI ci(dev, frameSize, depthType);
	_create(dev, ci);
}

void AXE_VkImageBase::_internal_create(AXE_VkDevice& dev, ::VkImageCreateInfo& ci) {
	destroy();
	
	_dev = &dev;
	
	_format = ci.format;
	_size	= ci.extent;
	_layout = ci.initialLayout;
	_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
}

void AXE_VkImage::_create(AXE_VkDevice& dev, ::VkImageCreateInfo& ci) {
	_internal_create(dev,ci);
	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = vkCreateImage(dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VmaImage::destroy() {
	if (_handle) {
		vmaDestroyImage(_dev->vmaAllocator(), _handle, _vmaAllocation);
		_handle 		= VK_NULL_HANDLE;
		_vmaAllocation	= VK_NULL_HANDLE;
		_dev			= nullptr;
	}
}

void AXE_VmaImage::createImage2D(AXE_VkDevice& dev, const Vec2i& frameSize, ColorType colorType, Int mipLevels) {
	ColorCI imgCI(dev, frameSize, colorType, mipLevels);
	
	::VmaAllocationCreateInfo allocCI = {}; // TODO simple for now
	allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocCI.usage = VMA_MEMORY_USAGE_AUTO;
	
	_create(dev, imgCI, allocCI);
}

void AXE_VmaImage::createDepthStencil(AXE_VkDevice& dev, const Vec2i& frameSize, RenderDepthType depthType) {
	DepthStencilCI imgCI(dev, frameSize, depthType);

	::VmaAllocationCreateInfo allocCI = {}; // TODO simple for now
	allocCI.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
	allocCI.usage = VMA_MEMORY_USAGE_AUTO;
	
	_create(dev, imgCI, allocCI);
}

void AXE_VmaImage::_create(AXE_VkDevice& dev, ::VkImageCreateInfo& imgCI, ::VmaAllocationCreateInfo& allocCI) {
	_internal_create(dev, imgCI);
	auto err = vmaCreateImage(_dev->vmaAllocator()
									, &imgCI
									, &allocCI
									, &_handle
									, &_vmaAllocation
									, nullptr);
	VkUtil::throwIfError(err);
}

void AXE_VkBufferBase::_internal_create(AXE_VkDevice& dev, ::VkBufferCreateInfo& ci) {
	destroy();
	
	_dev		 = &dev;
	_bufferSize  = ci.size;
	_bufferUsage = ci.usage;
}

void AXE_VmaBuffer::destroy() {
	if (_handle) {
		vmaDestroyBuffer(_dev->vmaAllocator(), _handle, _vmaAllocation);
		_handle 			= VK_NULL_HANDLE;
		_vmaAllocation		= VK_NULL_HANDLE;
		_vmaAllocationInfo	= {};
		_dev				= nullptr;
		_bMapped			= false;
	}
}

AXE_VmaBuffer::AXE_VmaBuffer(AXE_VmaBuffer && r) noexcept
	: _vmaAllocation(AXE_MOVE(r._vmaAllocation))
	, _vmaAllocationInfo(AXE_MOVE(r._vmaAllocationInfo))
	, _vmaUsage(AXE_MOVE(r._vmaUsage))
	, _bMapped(AXE_MOVE(r._bMapped))
{
	_handle					= r._handle;
	_dev					= r._dev;
	r._handle				= VK_NULL_HANDLE;
	r._dev					= nullptr;
	r._vmaAllocation		= VK_NULL_HANDLE;
	r._vmaAllocationInfo = {};
	r._vmaUsage				= VMA_MEMORY_USAGE_UNKNOWN;
	r._bMapped				= false;
}

void AXE_VmaBuffer::create(AXE_VkDevice& dev
						 , GpuBufferType bufferType
						 , Int bufferSize
) {
	if (bufferSize < 16) {
		bufferSize = 16;
	}
	
	::VkBufferUsageFlags		usage	 = 0;
	::VmaMemoryUsage			vmaUsage = VMA_MEMORY_USAGE_AUTO;
	::VmaAllocationCreateFlags	vmaFlag  = 0; // vmaFlag = 0 means No CPU access, needed after upload

	switch (bufferType) {
		case GpuBufferType::Vertex: {
			usage	 = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY; // Optimal for static data that the GPU reads frequently, and no CPU access.
			vmaFlag  = 0;
		}break;
		case GpuBufferType::Index: {
			usage	 = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			vmaFlag  = 0;
		}break;
		case GpuBufferType::Const: {
			usage	 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			vmaFlag  = 0;
		}break;
		case GpuBufferType::Structured: { // storage buffer
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
					| VK_BUFFER_USAGE_TRANSFER_SRC_BIT
					| VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			vmaUsage = VMA_MEMORY_USAGE_GPU_ONLY;
			vmaFlag  = 0;
		}break;
	//----		
		case GpuBufferType::StagingToGpu: {
			// Instead of creating/destroying staging buffer every time, create one persistent staging buffer for all uploads
				// just let VkBufferCreateInfo::size big enough
				// and just memcpy+vmaFlushAllocation without vmaMapMemory/vmaUnmapMemory (with VMA_ALLOCATION_CREATE_MAPPED_BIT)
			usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			vmaUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST; // VMA_MEMORY_USAGE_CPU_TO_GPU (Obsolete);
			vmaFlag = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT // uncached and write-combined
					| VMA_ALLOCATION_CREATE_MAPPED_BIT;						 // no need call vmaMapMemory/vmaUnmapMemory
		}break;
		case GpuBufferType::StagingToCpu: {
			usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			vmaUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST; // VMA_MEMORY_USAGE_GPU_TO_CPU(Obsolete);
			vmaFlag = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
					| VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}break;
	//----
		default:
			AXE_THROW;
	}
	
	::VkBufferCreateInfo bufCI {};
	bufCI.sType 				= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufCI.pNext					= nullptr;
	bufCI.flags					= 0;
	bufCI.size  				= bufferSize;
	bufCI.usage 				= usage;
	bufCI.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
	bufCI.queueFamilyIndexCount	= 0;
	bufCI.pQueueFamilyIndices	= nullptr;

	::VmaAllocationCreateInfo allocCI {};
	allocCI.flags 					 = vmaFlag;
	allocCI.usage 					 = vmaUsage;
	
	_create(dev, bufCI, allocCI);
};

void AXE_VmaBuffer::_create(AXE_VkDevice& dev, ::VkBufferCreateInfo& bufCI, ::VmaAllocationCreateInfo& allocCI) {
	_internal_create(dev, bufCI);
	_vmaUsage = allocCI.usage;
	
	auto err = ::vmaCreateBuffer(_dev->vmaAllocator()
										, &bufCI
										, &allocCI
										, &_handle
										, &_vmaAllocation
										, &_vmaAllocationInfo
	);
	VkUtil::throwIfError(err);

	if (allocCI.flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) {
		_bMapped = true;
	}
}

void AXE_VkFence::destroy() {
	if (_handle != VK_NULL_HANDLE) {
		// wait(eastl::nullopt);
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		::vkDestroyFence(*_dev, _handle, pAllocator);
		_handle = VK_NULL_HANDLE;
		_dev	= nullptr;
	}
}

AXE_VkFence::AXE_VkFence(AXE_VkFence && r) noexcept
	: _handle(AXE_MOVE(r._handle))
	, _dev(AXE_MOVE(r._dev)) 
{
	r._handle = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void AXE_VkFence::create(AXE_VkDevice& dev, bool signaled) {
	destroy();
	_dev = &dev;

	::VkFenceCreateInfo ci	= {};
	ci.sType 				= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	ci.pNext 				= nullptr;
	ci.flags 				= signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = ::vkCreateFence(dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

void AXE_VkFence::reset() {
	int fenceCount = 1;
	auto err = ::vkResetFences(*_dev, fenceCount, &_handle);
	VkUtil::throwIfError(err);
}

bool AXE_VkFence::check(bool doReset) {
	auto res = ::vkGetFenceStatus(*_dev, _handle);
	if (res == VK_NOT_READY)
		return false;

	if (doReset) {
		reset();
	}

	if (res == VK_SUCCESS)
		return true;

	VkUtil::throwIfError(res);
	return false;
}

bool AXE_VkFence::wait(const Opt<u64>& timeout) {
	u64 nanoseconds = timeout ? timeout.value() : UINT64_MAX;
	auto err = ::vkWaitForFences(*_dev, 1, &_handle, VK_TRUE, nanoseconds);
	if (err == VK_TIMEOUT)
		return false;

	VkUtil::throwIfError(err);
	return true;
}

void AXE_VkSemaphore::destroy() {
	if (_handle) {
		const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
		::vkDestroySemaphore(*_dev, _handle, pAllocator);
		_handle = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

AXE_VkSemaphore::AXE_VkSemaphore(AXE_VkSemaphore && r) noexcept
	: _handle(AXE_MOVE(r._handle))
	, _dev(AXE_MOVE(r._dev))
{
	r._handle = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void AXE_VkSemaphore::create(AXE_VkDevice& dev) {
	destroy();
	_dev = &dev;
	
	::VkSemaphoreCreateInfo ci = {};
	ci.sType 				   = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	ci.pNext 				   = nullptr;
	ci.flags 				   = 0;
	
	const VkAllocationCallbacks* pAllocator = VkUtil::allocCallbacks();
	auto err = ::vkCreateSemaphore(*_dev, &ci, pAllocator, &_handle);
	VkUtil::throwIfError(err);
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK