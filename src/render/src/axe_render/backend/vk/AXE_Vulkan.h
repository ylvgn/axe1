#pragma once

#if AXE_RENDER_HAS_VK

#include "Common_Vk.h"

namespace axe {

class AXE_VkInstance;
enum class AXE_VkQueueFamilyIndex : u32 { Invalid = INT_MAX };

struct AXE_VkApplicationInfo : public ::VkApplicationInfo {
	AXE_VkApplicationInfo();
}; // AXE_VkApplicationInfo


struct AXE_VkInstanceCreateInfo : public ::VkInstanceCreateInfo {
	AXE_VkInstanceCreateInfo();
	
	AXE_VkApplicationInfo appInfo;
	
	struct Props {
		Props();

		bool hasLayer		(StrView name) const;
		bool hasExtension	(StrView name) const;
		
		Vector<::VkLayerProperties, 64>		availableLayers;
		Vector<::VkExtensionProperties, 64>	availableExtensions;
	};
	Props props;
	
	void enableLayer		(StrLit name);
	bool tryEnableLayer		(StrLit name);
	
	void enableExtension	(StrLit name);
	bool tryEnableExtension	(StrLit name);
	
	void logInfo();
	
friend class AXE_VkInstance;
protected:
	Vector<const char*, 64> _enabledLayerNames;
	Vector<const char*, 64> _enabledExtensionNames;
}; // AXE_VkInstanceCreateInfo


class AXE_VkInstance : public NonCopyable {
public:
				::VkInstance handle()	{ return _instance; }
	operator	::VkInstance()			{ return _instance; }

	~AXE_VkInstance() noexcept { destroy(); }
	void destroy();

	void create(AXE_VkInstanceCreateInfo& createInfo);

private:
	VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
	VkBool32 onValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity
    							, VkDebugUtilsMessageTypeFlagsEXT				messageType
    							, const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData);

	static VKAPI_ATTR VkBool32 VKAPI_CALL s_debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT		messageSeverity
																	  , VkDebugUtilsMessageTypeFlagsEXT				messageType
																	  , const VkDebugUtilsMessengerCallbackDataEXT*	pCallbackData
																	  , void*										pUserData);
	
	::VkInstance _instance = VK_NULL_HANDLE;
}; // AXE_VkInstance


class AXE_VkPhysicalDevice : public NonCopyable {
public:
				::VkPhysicalDevice handle()		{ return _phyDev; }
	operator	::VkPhysicalDevice()			{ return _phyDev; }
	
				::VkInstance inst()	    { return _inst; }
	operator	::VkInstance()			{ return inst(); }
	
	AXE_VkPhysicalDevice() = default;
	AXE_VkPhysicalDevice(AXE_VkPhysicalDevice && r) noexcept;
	
	void create(Int index, ::VkInstance inst, ::VkPhysicalDevice phyDev);
	
	struct Properties : public AXE_VkBaseIOStructure {
		Properties();
		::VkPhysicalDeviceProperties2	base 	{};
		::VkPhysicalDeviceIDProperties	ID  	{};
	};
	
	const Properties&						props() const		{ return _props; }
	Span<::VkQueueFamilyProperties>			queueFamilyProps()	{ return _queueFamilyProps; }
	Span<::VkExtensionProperties>			extensionProps	()  { return _availableExtensions; }
	
	bool hasExtension(StrView name) const;
	
	Opt<AXE_VkQueueFamilyIndex> findQueueFamilyIndex(::VkQueueFlags hasFlags) const;
	Opt<AXE_VkQueueFamilyIndex> findGraphicFamilyIndex() const;
	Opt<AXE_VkQueueFamilyIndex> findComputeQueueFamilyIndex() const;
	
	explicit operator bool() const { return _phyDev != VK_NULL_HANDLE; }
private:
	Properties _props;
	
	Vector<::VkQueueFamilyProperties>	_queueFamilyProps;
	Vector<::VkExtensionProperties, 64>	_availableExtensions;
	
	Int					_index;
	::VkPhysicalDevice	_phyDev	= VK_NULL_HANDLE;
	::VkInstance		_inst	= VK_NULL_HANDLE;
}; // AXE_VkPhysicalDevice


class AXE_VkPhysicalDeviceList {
public:
	void create(::VkInstance inst);
	Span<AXE_VkPhysicalDevice>	devices()		{ return _devices; }
	AXE_VkPhysicalDevice*		defaultDevice() { return &_devices[0]; }

private:
	Vector<AXE_VkPhysicalDevice, 4>	_devices;
}; // AXE_VkPhysicalDeviceList


class AXE_VkDevice : public NonCopyable {
public:
			 ::VkDevice handle()	{ return _handle; }
	operator ::VkDevice()			{ return _handle; }

			 AXE_VkPhysicalDevice* physicalDevice();
	operator ::VkPhysicalDevice();
	
			 ::VkInstance inst();
	operator ::VkInstance();
	
	::VmaAllocator& vmaAllocator()	{ return _vmaAllocator; }
	
	~AXE_VkDevice() noexcept { destroy(); }
	void destroy();
	
	void create(Device_Vk& device);
	
	AXE_VkQueueFamilyIndex graphQueueFamilyIndex() const { return _graphQueueFamilyIndex; }
	
	void getQueue	  (class AXE_VkDeviceQueue& outQueue, AXE_VkQueueFamilyIndex queueFamilyIndex, u32 queueIndex);
	void getGraphQueue(class AXE_VkDeviceQueue& outQueue, u32 queueIndex = 0) { getQueue(outQueue, _graphQueueFamilyIndex, queueIndex); }

	Device_Vk* device() const { return _device; }
	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
	template<class OBJ>
	void setObjectDebugName(OBJ& objectHandle, StrView name) {
		auto t = VkUtil::s_objectType(objectHandle);
		_setObjectDebugName(t, objectHandle, name);
	}
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { setObjectDebugName(_handle, name); }
#endif
private:
	void _setObjectDebugName(VkObjectType objectType, void* objectHandle, StrView name);
	void _createAllocator();
	
	::VmaAllocator	_vmaAllocator	= VK_NULL_HANDLE;
	::VkDevice		_handle 		= VK_NULL_HANDLE;
	Device_Vk*		_device 		= nullptr;

	AXE_VkQueueFamilyIndex	_graphQueueFamilyIndex = AXE_VkQueueFamilyIndex::Invalid;
}; // AXE_VkDevice


class AXE_VkDeviceQueue : public NonCopyable {
public:
				const ::VkQueue& handle()	{ return _handle; }
	operator	const ::VkQueue&()			{ return _handle; }
	
	void submit(	Span<VkSubmitInfo> infos, VkFence fenceToSignal);
	
#if 0
	void submit(	const AXE_VkWaitSemaphores&	waitSemaphores,
					Span<VkCommandBuffer>		commandBuffers,
					Span<VkSemaphore>			signalSemaphores,
					VkFence						fenceToSignal = VK_NULL_HANDLE);
	
	void present(	Span<VkSemaphore>		waitSemaphores,
					Span<VkSwapchainKHR>	swapchains,
					Span<u32>				imageIndexInSwapchains) { return _present(waitSemaphores, swapchains, imageIndexInSwapchains); }

	void present(	VkSemaphore				waitSemaphore,
					VkSwapchainKHR			swapchain,
					Span<Int>				imageIndexInSwapchain);
#endif
	
	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
friend class AXE_VkDevice;
protected:
#if 0
	void _present(	Span<VkSemaphore>		waitSemaphores,
					Span<VkSwapchainKHR>	swapchains,
					Span<u32>				imageIndexInSwapchains);
#endif
	::VkQueue		_handle = VK_NULL_HANDLE;
	AXE_VkDevice*	_dev	= nullptr;
}; // AXE_VkDeviceQueue


class AXE_VkSurfaceKHR : public NonCopyable {
public:
			 const ::VkSurfaceKHR& handle() { return _surface; }
	operator const ::VkSurfaceKHR&()		{ return _surface; }

	~AXE_VkSurfaceKHR() noexcept { destroy(); }
	void destroy();
	
#if AXE_OS_WINDOWS
	void create_Win32(AXE_VkDevice& dev, ::HINSTANCE hInstance, ::HWND hwnd);
#endif
	
	::VkSurfaceCapabilitiesKHR getCapabilities();
	
	Span<::VkPresentModeKHR>	supportedPresentModes() { return _availablePresentModes; }
	Span<::VkSurfaceFormatKHR>	supportedFormats()		{ return _availableFormats; }
	
	bool isSupportFormat(const VkSurfaceFormatKHR& f) { return isSupportFormat(f.format, f.colorSpace); }
	bool isSupportFormat(VkFormat format, VkColorSpaceKHR colorSpace);
	
	bool checkQueueFamilySupportPresent (AXE_VkQueueFamilyIndex queueFamilyIndex) const;
	Opt<AXE_VkQueueFamilyIndex> findQueueFamilySupportPresent() const;
	
//	AXE_VkQueueFamilyIndex	 graphQueueFamilyIndex	() const { return _graphQueueFamilyIndex; }
//	AXE_VkQueueFamilyIndex	 presentQueueFamilyIndex() const { return _presentQueueFamilyIndex; }
	
//	void getGraphQueue  (AXE_VkDeviceQueue& outQueue, u32 queueIndex) { _dev->getQueue(outQueue, _graphQueueFamilyIndex,   queueIndex); }
//	void getPresentQueue(AXE_VkDeviceQueue& outQueue, u32 queueIndex) { _dev->getQueue(outQueue, _presentQueueFamilyIndex, queueIndex); }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_surface, name); }
#endif
private:
	void _onCreate();

	::VkSurfaceKHR		_surface = VK_NULL_HANDLE;
	AXE_VkDevice*		_dev  = nullptr;
	
	Vector<::VkSurfaceFormatKHR, 32>	_availableFormats;
	Vector<  ::VkPresentModeKHR, 32>	_availablePresentModes;

//	AXE_VkQueueFamilyIndex   _graphQueueFamilyIndex	= AXE_VkQueueFamilyIndex::Invalid;
//	AXE_VkQueueFamilyIndex _presentQueueFamilyIndex	= AXE_VkQueueFamilyIndex::Invalid;
}; // AXE_VkSurfaceKHR


class AXE_VkSwapchainKHR : public NonCopyable {
public:
			 const ::VkSwapchainKHR& handle()	{ return _handle; }
	operator const ::VkSwapchainKHR&()			{ return _handle; }

	AXE_VkSwapchainKHR() = default;
	~AXE_VkSwapchainKHR() noexcept { destroy(); }

	void destroy();
	void create(AXE_VkDevice&				dev,
				AXE_VkSurfaceKHR&			surface,
				const ::VkSurfaceFormatKHR&	format,
				Int							imageCount_,
				::VkPresentModeKHR			presentMode,
				AXE_VkSwapchainKHR*			oldSwapchain = nullptr);

	void getImages(Vector<VkImage>& outImages);

	VkResult acquireNextImage(	u32& outImageIndex, 
								VkSemaphore semaphoreToSignal, 
								VkFence fenceToSignal, 
								u64 timeoutNanoseconds = UINT64_MAX);
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
private:
	::VkSwapchainKHR	_handle = VK_NULL_HANDLE;
	AXE_VkDevice*		_dev	= nullptr;
}; // AXE_VkSwapchainKHR


class AXE_VkCommandPool : public NonCopyable {
public:
				const ::VkCommandPool& handle()	{ return _handle; }
	operator	const ::VkCommandPool&()		{ return _handle; }

	AXE_VkCommandPool() = default;
	~AXE_VkCommandPool() { destroy(); }

	void destroy();
	void create(AXE_VkDevice& dev, AXE_VkQueueFamilyIndex queueFamilyIndex);

	AXE_VkDevice*	device() { return _dev; }

	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
private:
	VkCommandPool	_handle = VK_NULL_HANDLE;
	AXE_VkDevice*	_dev	= nullptr;
}; // AXE_VkCommandPool


class AXE_VkCommandBuffer : public NonCopyable {
public:
				const ::VkCommandBuffer& handle()	{ return _handle; }
	operator	const ::VkCommandBuffer&()			{ return _handle; }

	~AXE_VkCommandBuffer() { destroy(); }
	
	AXE_VkCommandBuffer() = default;
	AXE_VkCommandBuffer(AXE_VkCommandBuffer && r) noexcept;
	
	void destroy();
	void create(AXE_VkCommandPool& cmdPool);

	void resetAndReleaseResource();

	void beginCommand();
	void endCommand();

	void pipelineBarrier(	AXE_VkQueueFamilyIndex srcQueueFamilyIndex,
							AXE_VkQueueFamilyIndex dstQueueFamilyIndex,
							VkImage image);

	//void debugLabelBegin(const String& name, const Color4f& color = Color4f(0,0,0,0));
	//void debugLabelEnd();

	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_cmdPool && _cmdPool->device()) _cmdPool->device()->setObjectDebugName(_handle, name); }
#endif
private:
	::VkCommandBuffer	_handle = VK_NULL_HANDLE;
	AXE_VkCommandPool*	_cmdPool = nullptr;
}; // AXE_VkCommandBuffer


class AXE_VkImageView : public NonCopyable {
public:
				const ::VkImageView& handle()	{ return _handle; }
	operator	const ::VkImageView&()			{ return _handle; }

	AXE_VkImageView() = default;
	~AXE_VkImageView() noexcept { destroy(); }

	void destroy();
	
	void create(AXE_VkDevice& dev, VkImage image, VkFormat format);
	void create(AXE_VkDevice& dev, VkImageViewCreateInfo& ci);
	
	::VkFormat format() const { return _format; }
	
	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
private:
	::VkImageView	_handle = VK_NULL_HANDLE;
	AXE_VkDevice*	_dev	= nullptr;
	VkFormat		_format = VK_FORMAT_UNDEFINED;
}; // AXE_VkImageView

struct VkImageCreateInfo_Color : public ::VkImageCreateInfo {
	VkImageCreateInfo_Color(AXE_VkDevice& dev, const Vec2i& frameSize, ColorType colorType, Int mipLevels);
}; // VkImageCreateInfo_Color
struct VkImageCreateInfo_DepthStencil : public ::VkImageCreateInfo {
	VkImageCreateInfo_DepthStencil(AXE_VkDevice& dev, const Vec2i& frameSize, RenderDepthType depthType);
}; // VkImageCreateInfo_DepthStencil

class AXE_VkImageBase : public NonCopyable {
public:
	virtual ~AXE_VkImageBase() = default;
	virtual void destroy() = 0;
	
				const ::VkImage& handle() const	{ return _handle; }
	operator	const ::VkImage&()				{ return _handle; }
	
	AXE_VkDevice*		device()		{ return _dev; }
	const ::VkExtent3D&	size() const	{ return _size; }
	::VkFormat			format()		{ return _format; }
	::VkImageLayout		layout() const	{ return _layout; }
	
	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
protected:
	using DepthStencilCI = VkImageCreateInfo_DepthStencil;
	using ColorCI		 = VkImageCreateInfo_Color;
	
	AXE_VkImageBase() noexcept = default;
	void _internal_create(AXE_VkDevice& dev, ::VkImageCreateInfo& ci);

	::VkImage				_handle	= VK_NULL_HANDLE;
	AXE_VkDevice*			_dev	= nullptr;
	::VkExtent3D			_size	= {};
	::VkFormat				_format	= VK_FORMAT_UNDEFINED;
	::VkImageLayout			_layout	= VK_IMAGE_LAYOUT_UNDEFINED;
	::VkPipelineStageFlags	_stage	= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
}; // AXE_VkImageBase

class AXE_VkImage : public AXE_VkImageBase {
	AXE_TYPE_INFO(AXE_VkImage, AXE_VkImageBase)
public:
	AXE_VkImage() = default;
	~AXE_VkImage() { destroy(); }

	virtual void destroy() final;
	
	void createImage2D		(AXE_VkDevice& dev, const Vec2i& frameSize, ColorType colorType, Int mipLevels);
	void createDepthStencil	(AXE_VkDevice& dev, const Vec2i& frameSize, RenderDepthType depthType);
private:
	void _create			(AXE_VkDevice& dev, ::VkImageCreateInfo& ci);
}; // AXE_VkImage

class AXE_VmaImage : public AXE_VkImageBase {
	AXE_TYPE_INFO(AXE_VmaImage, AXE_VkImageBase)
public:
	AXE_VmaImage() = default;
	~AXE_VmaImage() { destroy(); }

	virtual void destroy() final;

	void createImage2D		(AXE_VkDevice& dev, const Vec2i& frameSize, ColorType colorType, Int mipLevels);
	void createDepthStencil	(AXE_VkDevice& dev, const Vec2i& frameSize, RenderDepthType depthType);
	
private:
	void _create(AXE_VkDevice& dev, ::VkImageCreateInfo& imgCI, ::VmaAllocationCreateInfo& allocCI);
	
	::VmaAllocation _vmaAllocation = VK_NULL_HANDLE;
}; // AXE_VmaImage


class AXE_VkBufferBase : public NonCopyable {
public:
	virtual ~AXE_VkBufferBase() = default;
	virtual void destroy() = 0;

				const ::VkBuffer& handle() const	{ return _handle; }
	operator	const ::VkBuffer&()					{ return _handle; }

	AXE_VkDevice*			device()				{ return _dev; }
	const ::VkDeviceSize&	bufferSize() const		{ return _bufferSize; }
	::VkBufferUsageFlags	bufferUsage()			{ return _bufferUsage; }
	
	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }

#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
protected:
	AXE_VkBufferBase() noexcept = default;
	void _internal_create(AXE_VkDevice& dev, ::VkBufferCreateInfo& ci);

	::VkBuffer				_handle			= VK_NULL_HANDLE;
	AXE_VkDevice*			_dev			= nullptr;
	::VkDeviceSize			_bufferSize		= 0;
	::VkBufferUsageFlags	_bufferUsage	= 0;
}; // AXE_VkBufferBase


class AXE_VmaBuffer : public AXE_VkBufferBase {
	AXE_TYPE_INFO(AXE_VmaBuffer, AXE_VkBufferBase)
public:
	AXE_VmaBuffer() = default;
	~AXE_VmaBuffer() { destroy(); }
	
	virtual void destroy() final;
	
	AXE_VmaBuffer(AXE_VmaBuffer && r) noexcept;
	
	void create(AXE_VkDevice&  dev,
				GpuBufferType  bufferType,
				Int            bufferSize);

	bool bOutOfBufferSize(IntRange uploadRange) {
		return !IntRange(/*bufferSize()*/ _vmaAllocationInfo.size).contains(uploadRange);
	}
	
	bool bOutOfBufferSize(Int mappedOffset, Int mappedDataSizeInBytes) {
		IntRange range;
		range.setStartAndSize(mappedOffset, mappedDataSizeInBytes);
		return bOutOfBufferSize(range);
	}
	
	void unmapMemory() {
		if (_bMapped)
			return;
		vmaUnmapMemory(_dev->vmaAllocator(), _vmaAllocation);
	}
	
	void mapMemory(byte*& outMappedPtr) {
		outMappedPtr = nullptr;
		if (_bMapped) {
			outMappedPtr = static_cast<byte*>(_vmaAllocationInfo.pMappedData);
			return;
		}
		vmaMapMemory(_dev->vmaAllocator(), _vmaAllocation, reinterpret_cast<void**>(&outMappedPtr));
	}

	ByteSpan _mapMemory(IntRange range) { // Note: Do not use ByteSpan.data after called 'vmaUnmapMemory', A dangling ptr.
		if (bOutOfBufferSize(range))
			AXE_THROW_ERROR("out of range: AXE_VkBuffer::mapMemory");
		
		byte* outMappedPtr;
		mapMemory(outMappedPtr);
		return ByteSpan(outMappedPtr + range.start(), range.size());
	}
	
	void flushMappedMemoryRanges(IntRange range) { flushMappedMemoryRanges(range.start(), range.size()); }
	void flushMappedMemoryRanges(Int mappedOffset, Int mappedDataSizeInBytes) {
		vmaFlushAllocation(_dev->vmaAllocator(), _vmaAllocation, mappedOffset, mappedDataSizeInBytes);
	}
	
	void invalidateMappedMemoryRanges(IntRange range) { invalidateMappedMemoryRanges(range.start(), range.size()); }
	void invalidateMappedMemoryRanges(Int mappedOffset, Int mappedDataSizeInBytes) {
		vmaFlushAllocation(_dev->vmaAllocator(), _vmaAllocation, mappedOffset, mappedDataSizeInBytes);
	}
	
	class ScopedVmaMapMemory : public NonCopyable {
	public:
		ScopedVmaMapMemory(AXE_VmaBuffer& owner, Int mappedOffset, Int mappedDataSizeInBytes)
			: _owner(&owner)
			, _mappedOffset(mappedOffset)
			, _mappedDataSizeInBytes(mappedDataSizeInBytes)
		{
			if (_owner) {
				if (_owner->bOutOfBufferSize(mappedOffset, mappedDataSizeInBytes)) {
					AXE_THROW_ERROR("out of range: ScopedVmaMapMemory:ctor");
				}
				_owner->mapMemory(_pMappedData);
			}
		}

		~ScopedVmaMapMemory() {
			if (_owner) {
				_owner->flushMappedMemoryRanges(_mappedOffset, _mappedDataSizeInBytes);
				_owner->unmapMemory();	
			}
		}
			
		ScopedVmaMapMemory(ScopedVmaMapMemory&& other) = delete; // Prevent move
			
		void copyData(ByteSpan srcData, Int dstOffset = 0) {
			if (_owner) {
				if (_owner->bOutOfBufferSize(IntRange(_mappedOffset + dstOffset + srcData.size_bytes()))) {
					AXE_THROW_ERROR("out of range: ScopedVmaMapMemory:copyData");
				}
				memcpy(_pMappedData + _mappedOffset + dstOffset, srcData.data(), srcData.size_bytes());
			}
		}
			
		explicit operator bool() const { return _owner != nullptr; }
			
	private:
		AXE_VmaBuffer*  _owner		 = nullptr;
		byte*			_pMappedData = nullptr;
		Int 			_mappedOffset = 0;
		Int 			_mappedDataSizeInBytes = 0;
	}; // ScopedVmaMapMemory
	AXE_NODISCARD AXE_INLINE ScopedVmaMapMemory mapMemory(Int mappedDataSizeInBytes) { return mapMemory(0, mappedDataSizeInBytes); }
	AXE_NODISCARD AXE_INLINE ScopedVmaMapMemory mapMemory(IntRange range) { return mapMemory(range.start(), range.size()); }
	AXE_NODISCARD AXE_INLINE ScopedVmaMapMemory mapMemory(Int mappedOffset, Int mappedDataSizeInBytes) {
		return ScopedVmaMapMemory(*this, mappedOffset, mappedDataSizeInBytes);
	}
	
	const ::VmaAllocationInfo&	vmaAllocationInfo() const	{ return _vmaAllocationInfo; }
	::VmaMemoryUsage			vmaUsage()					{ return _vmaUsage; }
	
private:
	void _create(AXE_VkDevice& dev, ::VkBufferCreateInfo& bufCI, ::VmaAllocationCreateInfo& allocCI);
	
	::VmaAllocation		_vmaAllocation		= VK_NULL_HANDLE;
	::VmaAllocationInfo _vmaAllocationInfo	= {};
	::VmaMemoryUsage    _vmaUsage			= VMA_MEMORY_USAGE_UNKNOWN;

	bool _bMapped		= false;
}; // AXE_VmaBuffer


class AXE_VkFence : public NonCopyable {
public:
				const ::VkFence& handle()	{ return _handle; }
	operator	const ::VkFence&() 			{ return _handle; }

	~AXE_VkFence() { destroy(); }
	
	AXE_VkFence() = default;
	AXE_VkFence(AXE_VkFence && r) noexcept;

	void destroy();
	void create(AXE_VkDevice& dev, bool signaled);

	void reset();
	bool wait(const Opt<u64>& timeoutMs);

	bool check(bool doReset);

	explicit operator bool() const { return _handle != VK_NULL_HANDLE; }
	
#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
private:
	::VkFence		_handle = VK_NULL_HANDLE;
	AXE_VkDevice*	_dev	= nullptr;
}; // AXE_VkFence


class AXE_VkSemaphore : public NonCopyable {
public:
				const ::VkSemaphore& handle() 	{ return _handle; }
	operator	const ::VkSemaphore&() 			{ return _handle; }

	~AXE_VkSemaphore() { destroy(); }
	AXE_VkSemaphore() = default;
	
	AXE_VkSemaphore(AXE_VkSemaphore && r) noexcept;

	void create(AXE_VkDevice& dev);
	void destroy();

#if AXE_RENDER_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif
private:
	::VkSemaphore	_handle = VK_NULL_HANDLE;
	AXE_VkDevice*	_dev = nullptr;
}; // AXE_VkSemaphore

} // namespace axe

#endif // AXE_TRY_USE_VK
