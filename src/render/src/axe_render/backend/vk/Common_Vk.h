#pragma once

#if AXE_RENDER_HAS_VK

#include "AXE_Volk.h"

#include <vulkan/vk_enum_string_helper.h>

#if AXE_OS_WINDOWS
	#include <vulkan/vulkan_win32.h>
#endif

#include "vk_mem_alloc.h"

#if AXE_OS_WINDOWS
	#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>
#endif

#include <axe_render/RenderCommonBase.h>


#define AXE_MACRO_OP(E) \
	template<> \
	struct fmt::formatter<::E> { \
		static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); } \
		static auto format(const ::E& v, fmt::format_context& ctx) { \
			return fmt::format_to(ctx.out(), "{}", ::axe::StrView_c_str(string_##E(v))); \
		} \
	}; \
//----
	AXE_MACRO_OP(VkResult)
	AXE_MACRO_OP(VkPhysicalDeviceType)
#undef AXE_MACRO_OP

namespace axe {

class Renderer_Vk;
class Capabilities_Vk;
class Device_Vk;

	
#if 0
#pragma mark ========= AXE_VkBaseIOStructure ============
#endif
struct AXE_VkBaseIOStructure {
protected:
	void _setNext(VkBaseOutStructure* p);
	void _setNext( VkBaseInStructure* p);
private:
	template<class T> static inline
	void _s_setNextImpl(T*& io, T* p) {
		AXE_ASSERT(p != nullptr);
		if (!io) {
			io = p;
		} else {
			io->pNext = p;
			io = axe_const_cast(io->pNext);
		}
	}
	::VkBaseOutStructure* _out = nullptr;
	::VkBaseInStructure*  _in  = nullptr;
};


#if 0
#pragma mark ========= VkUtil ============
#endif
class VkUtil {
public:
	static bool checkResult(::VkResult err);
	static void throwIfError(::VkResult err) {
		if (_checkError(err)) {
			AXE_THROW_ERROR("VkResult: {}", err);
		}
	}

	static Renderer_Vk*		renderer();
	static Device_Vk*		rootDevice();
	static class AXE_VkAllocatorCallbacks* allocCallbacks();
	
	static ::VkPrimitiveTopology	getVkPrimitiveTopology(RenderPrimitiveType t);
	static ::VkFormat 				getVkColorType(ColorType type);
	static ::VkFormat 				getVkDataType (RenderDataType type);
	static ::VkFormat				getVkDepthType(RenderDepthType v);
	
	static bool formatHasDepth	(VkFormat f);
	static bool formatHasStencil(VkFormat f);
	
	static void convert(i64& o, const u8 i[VK_UUID_SIZE]) { memcpy(&o, i, sizeof(decltype(o))); }
	
	static void convert(String& dstUUID, const u8 srcUUID[VK_UUID_SIZE]) {
		static const char kHexChars[] = "0123456789abcdef";
		dstUUID.resize(VK_UUID_SIZE * 2);
    
		for (uint32_t i = 0, j = 0; i < VK_UUID_SIZE; i++) {
			dstUUID[j++] = (kHexChars[srcUUID[i] >> 4]);
			dstUUID[j++] = (kHexChars[srcUUID[i] & 0x0F]);
		}
	}
	
	static void convert(uint8_t dstUUID[VK_UUID_SIZE], const StrView srcUUID) {
		AXE_ASSERT(srcUUID.length() == VK_UUID_SIZE * 2);
    
		for (size_t i = 0; i < 16; i++) {
			StrView byteStr = srcUUID.substr(i * 2, 2);
			StringUtil::tryParse(byteStr, dstUUID[i]);
		}
	}
	
	static u32		castUInt32(i32 v) 		{ return axe_safe_cast_from(v); }
	
	static u32		castUInt32(i64 v) 		{ return axe_safe_cast_from(v); }
	static i32		castInt32(i64 v) 		{ return axe_safe_cast_from(v); }
	
	static u32		castUInt32(size_t v) 	{ return axe_safe_cast_from(v); }
	static i32		castInt32(size_t v) 	{ return axe_safe_cast_from(v); }

	static Vec2i 	castVec2i(const VkOffset2D& r) { return Vec2i(axe_safe_cast_from(r.x), axe_safe_cast_from(r.y)); }
	static Vec2f 	castVec2f(const VkOffset2D& r) { return Vec2f(axe_safe_cast_from(r.x), axe_safe_cast_from(r.y)); }

	static Vec2i 	castVec2i(const VkExtent2D& r) { return Vec2i(axe_safe_cast_from(r.width), axe_safe_cast_from(r.height)); }
	static Vec2f 	castVec2f(const VkExtent2D& r) { return Vec2f(axe_safe_cast_from(r.width), axe_safe_cast_from(r.height)); }

	static Vec3i 	castVec3i(const VkOffset3D& r) { return Vec3i(axe_safe_cast_from(r.x), axe_safe_cast_from(r.y), axe_safe_cast_from(r.z)); }
	static Vec3f 	castVec3f(const VkOffset3D& r) { return Vec3f(axe_safe_cast_from(r.x), axe_safe_cast_from(r.y), axe_safe_cast_from(r.z)); }
	
	static Vec3i 	castVec3i(const VkExtent3D& r) { return Vec3i(axe_safe_cast_from(r.width), axe_safe_cast_from(r.height), axe_safe_cast_from(r.depth)); }
	static Vec3f 	castVec3f(const VkExtent3D& r) { return Vec3f(axe_safe_cast_from(r.width), axe_safe_cast_from(r.height), axe_safe_cast_from(r.depth)); }

	static ::VkDeviceSize	castVkDeviceSize(Int v)	{ return axe_safe_cast_from(v); }

	template<class R>
	static VkOffset2D castVkOffset2D(const Vec2<R>& v) {
		::VkOffset2D o;
		o.x = axe_safe_cast_from(v.x);
		o.y = axe_safe_cast_from(v.y);
		return o;
	}

	template<class R>
	static ::VkExtent2D	castVkExtent2D(const Vec2<R>& v) {
		::VkExtent2D o;
		o.width  = axe_safe_cast_from(v.x);
		o.height = axe_safe_cast_from(v.y);
		return o;
	}

	template<class R>
	static ::VkRect2D	castVkRect2D(const Rect2<R>& s) {
		::VkRect2D o;
		o.offset = castVkOffset2D(s.pos);
		o.extent = castVkExtent2D(s.size);
		return o;
	}
	
AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wold-style-cast")
	static u32 makeApiVersion(u32 variant, u32 major, u32 minor, u32 patch) { return VK_MAKE_API_VERSION(variant, major, minor, patch); }
	static u32 makeApiVersion(             u32 major, u32 minor, u32 patch)	{ return VK_MAKE_VERSION    (         major, minor, patch); }
	static u32 makeApiVersionVariant(u32 vkApiVersion) 						{ return VK_API_VERSION_VARIANT(vkApiVersion); }
	static u32   makeApiVersionMajor(u32 vkApiVersion) 						{ return   VK_API_VERSION_MAJOR(vkApiVersion); }
	static u32   makeApiVersionMinor(u32 vkApiVersion) 						{ return   VK_API_VERSION_MINOR(vkApiVersion); }
	static u32   makeApiVersionPatch(u32 vkApiVersion) 						{ return   VK_API_VERSION_PATCH(vkApiVersion); }
AXE_GCC_WARNING_POP()
	
	template<class OBJ>
	constexpr static ::VkObjectType s_objectType();

	template<class OBJ>
	constexpr static ::VkObjectType s_objectType(const OBJ& obj) {
		return s_objectType<OBJ>();
	}
	
private:
	static bool _checkError(::VkResult err) {
		return err != VK_SUCCESS; // if got error, return true
	}
}; // VkUtil
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(VkUtil);
	
// In alphabet order
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkBuffer		>() { return VK_OBJECT_TYPE_BUFFER;			}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkCommandBuffer	>() { return VK_OBJECT_TYPE_COMMAND_BUFFER;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkCommandPool	>() { return VK_OBJECT_TYPE_COMMAND_POOL;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkDescriptorSet	>() { return VK_OBJECT_TYPE_DESCRIPTOR_SET;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkDevice		>() { return VK_OBJECT_TYPE_DEVICE;			}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkDeviceMemory	>() { return VK_OBJECT_TYPE_DEVICE_MEMORY;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkFence			>() { return VK_OBJECT_TYPE_FENCE;			}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkFramebuffer	>() { return VK_OBJECT_TYPE_FRAMEBUFFER;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkImage			>() { return VK_OBJECT_TYPE_IMAGE;			}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkImageView		>() { return VK_OBJECT_TYPE_IMAGE_VIEW;		}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkPipeline		>() { return VK_OBJECT_TYPE_PIPELINE;		}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkPipelineCache	>() { return VK_OBJECT_TYPE_PIPELINE_CACHE;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkQueue			>() { return VK_OBJECT_TYPE_QUEUE;			}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkRenderPass	>() { return VK_OBJECT_TYPE_RENDER_PASS;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkSampler		>() { return VK_OBJECT_TYPE_SAMPLER;		}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkSemaphore		>() { return VK_OBJECT_TYPE_SEMAPHORE;		}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkShaderModule	>() { return VK_OBJECT_TYPE_SHADER_MODULE;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkSurfaceKHR	>() { return VK_OBJECT_TYPE_SURFACE_KHR;	}
template<> AXE_INLINE constexpr ::VkObjectType VkUtil::s_objectType<VkSwapchainKHR	>() { return VK_OBJECT_TYPE_SWAPCHAIN_KHR;	}


inline
bool VkUtil::checkResult(::VkResult err) {
	if (_checkError(err))
	{
		AXE_LOG_WARN("VkResult: {}", err);
		return false;
	}
	return true;
}

inline
::VkPrimitiveTopology VkUtil::getVkPrimitiveTopology(RenderPrimitiveType t) {
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case SRC::Lines:		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case SRC::Triangles:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	//---
		default:
			AXE_THROW;
	}
}

inline
::VkFormat VkUtil::getVkColorType(ColorType type) {
	using SRC = ColorType;
	switch (type) {
		case SRC::HSVAf: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAf: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case SRC::RGBAh: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case SRC::RGBAb: return VK_FORMAT_R8G8B8A8_UNORM;
	//---
//		case SRC::RGBb: 
		case SRC::RGb:	 return VK_FORMAT_R8G8_UNORM;
		case SRC::Rb:	 return VK_FORMAT_R8_UNORM;
//		case SRC::Ab:	 return VK_FORMAT_A8_UNORM;
		case SRC::Lb:	 return VK_FORMAT_R8_UNORM;
//		case SRC::Af:	 return VK_FORMAT_R32_SFLOAT;
		case SRC::Lf:	 return VK_FORMAT_R32_SFLOAT;
		case SRC::LAf:	 return VK_FORMAT_R32G32_SFLOAT;
		case SRC::LAb:	 return VK_FORMAT_R8G8_UNORM;
	//---
		case SRC::BC1:		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case SRC::BC2:		return VK_FORMAT_BC2_UNORM_BLOCK;
		case SRC::BC3:		return VK_FORMAT_BC3_UNORM_BLOCK;
		case SRC::BC4:		return VK_FORMAT_BC4_UNORM_BLOCK;
		case SRC::BC5:		return VK_FORMAT_BC5_UNORM_BLOCK;
		case SRC::BC6h:		return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case SRC::BC7:		return VK_FORMAT_BC7_UNORM_BLOCK;
	//---
		default:
			AXE_THROW;
	}
}

inline
::VkFormat VkUtil::getVkDataType(RenderDataType type) {
	using SRC = RenderDataType;
	switch (type) {
		case SRC::UInt8:		return VK_FORMAT_R8_UINT; break;
		case SRC::UInt8x2:		return VK_FORMAT_R8G8_UINT; break;
		case SRC::UInt8x3:		return VK_FORMAT_R8G8B8_UINT; break;
		case SRC::UInt8x4:		return VK_FORMAT_R8G8B8A8_UINT; break;
	//---
		case SRC::UInt16:		return VK_FORMAT_R16_UINT;
		case SRC::UInt16x2:		return VK_FORMAT_R16G16_UINT;
		case SRC::UInt16x3:		return VK_FORMAT_R16G16B16_UINT;
		case SRC::UInt16x4:		return VK_FORMAT_R16G16B16A16_UINT;
	//---
		case SRC::UInt32:		return VK_FORMAT_R32_UINT;
		case SRC::UInt32x2:		return VK_FORMAT_R32G32_UINT;
		case SRC::UInt32x3:		return VK_FORMAT_R32G32B32_UINT;
		case SRC::UInt32x4:		return VK_FORMAT_R32G32B32A32_UINT;
	//---
		case SRC::Int8:			return VK_FORMAT_R8_SINT;
		case SRC::Int8x2:		return VK_FORMAT_R8G8_SINT;
		case SRC::Int8x3:		return VK_FORMAT_R8G8B8_SINT;
		case SRC::Int8x4:		return VK_FORMAT_R8G8B8A8_SINT;
	//---
		case SRC::Int16:		return VK_FORMAT_R16_SINT;
		case SRC::Int16x2:		return VK_FORMAT_R16G16_SINT;
		case SRC::Int16x3:		return VK_FORMAT_R16G16B16_SINT;
		case SRC::Int16x4:		return VK_FORMAT_R16G16B16A16_SINT;
	//---
		case SRC::Int32:		return VK_FORMAT_R32_SINT;
		case SRC::Int32x2:		return VK_FORMAT_R32G32_SINT;
		case SRC::Int32x3:		return VK_FORMAT_R32G32B32_SINT;
		case SRC::Int32x4:		return VK_FORMAT_R32G32B32A32_SINT;
	//---
		case SRC::UNorm8:		return VK_FORMAT_R8_UNORM;
		case SRC::UNorm8x2:		return VK_FORMAT_R8G8_UNORM;
		case SRC::UNorm8x3:		return VK_FORMAT_R8G8B8_UNORM;
		case SRC::UNorm8x4:		return VK_FORMAT_R8G8B8A8_UNORM;
	//---
		case SRC::UNorm16:		return VK_FORMAT_R16_UNORM;
		case SRC::UNorm16x2:	return VK_FORMAT_R16G16_UNORM;
		case SRC::UNorm16x3:	return VK_FORMAT_R16G16B16_UNORM;
		case SRC::UNorm16x4:	return VK_FORMAT_R16G16B16A16_UNORM;
	//---
		case SRC::SNorm8:		return VK_FORMAT_R8_SNORM;
		case SRC::SNorm8x2:		return VK_FORMAT_R8G8_SNORM;
		case SRC::SNorm8x3:		return VK_FORMAT_R8G8B8_SNORM;
		case SRC::SNorm8x4:		return VK_FORMAT_R8G8B8A8_SNORM;
	//---
		case SRC::SNorm16:		return VK_FORMAT_R16_SNORM;
		case SRC::SNorm16x2:	return VK_FORMAT_R16G16_SNORM;
		case SRC::SNorm16x3:	return VK_FORMAT_R16G16B16_SNORM;
		case SRC::SNorm16x4:	return VK_FORMAT_R16G16B16A16_SNORM;
	//---
		case SRC::Float16:		return VK_FORMAT_R16_SFLOAT; break;
		case SRC::Float16x2:	return VK_FORMAT_R16G16_SFLOAT; break;
		case SRC::Float16x3:	return VK_FORMAT_R16G16B16_SFLOAT; break;
		case SRC::Float16x4:	return VK_FORMAT_R16G16B16A16_SFLOAT; break;
	//---
		case SRC::Float32:		return VK_FORMAT_R32_SFLOAT; break;
		case SRC::Float32x2:	return VK_FORMAT_R32G32_SFLOAT; break;
		case SRC::Float32x3:	return VK_FORMAT_R32G32B32_SFLOAT; break;
		case SRC::Float32x4:	return VK_FORMAT_R32G32B32A32_SFLOAT; break;
	//---
		default:
			AXE_THROW;
	}
}

inline
VkFormat VkUtil::getVkDepthType(RenderDepthType v) {
	using SRC = RenderDepthType;
	switch (v) {
		case SRC::None:							return VK_FORMAT_UNDEFINED;
		case SRC::Depth_Float32:				return VK_FORMAT_D32_SFLOAT;
		case SRC::Depth_Float32_Stencil_UInt8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case SRC::Depth_UNorm16:				return VK_FORMAT_D16_UNORM;
//		case SRC::Depth_UNorm16_Stencil_UInt8:	return VK_FORMAT_D16_UNORM_S8_UINT;
		case SRC::Depth_UNorm24_Stencil_UInt8:	return VK_FORMAT_D24_UNORM_S8_UINT;
//		case SRC::Depth_Stencil_UInt8:			return VK_FORMAT_S8_UINT;
	//---
		default:
			AXE_THROW;
	}
}
	
inline
bool VkUtil::formatHasDepth(VkFormat f) {
	switch (f) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default: return false;
	}
}

inline
bool VkUtil::formatHasStencil(VkFormat f) {
	switch (f) {
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default: return false;
	}
}
	
#if 0
#pragma mark ========= AXE_VkAllocatorCallbacks ============
#endif
class AXE_VkAllocatorCallbacks : public VkAllocationCallbacks {
	AXE_TYPE_INFO(AXE_VkAllocatorCallbacks, VkAllocationCallbacks)
public:
	AXE_VkAllocatorCallbacks() { AXE_ASSERT_NOT_IMPLEMENTED(); }
}; // AXE_VkAllocatorCallbacks

} // namespace axe

#endif // AXE_RENDER_HAS_VK