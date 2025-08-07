#pragma once

#include "RenderDataType.h"

namespace axe {

class EditMesh;

class Material;
class Material_CreateDesc;
class MaterialPass;

enum class MonitorDisplayMode : u8;

class Renderer;
class Renderer_CreateDesc;
class RenderAdapterInfo;
class RenderBuiltInAssets;
class RenderCapabilities_Info;
class RenderCapabilities;
class RenderCommandBuffer;
class RenderCommand_ClearFrameBuffers;
class RenderCommand_DrawCall;
class RenderCommand_SetViewport;
class RenderCommand_SetScissorRect;
class RenderCommand_SwapBuffers;
class RenderContext;
class RenderContext_CreateDesc;
class RenderDataTypeUtil;
class RenderDeviceObject;
class RenderDevice;
class RenderDevice_CreateDesc;
class RenderFence;
class RenderGpuBuffer;
class RenderGpuBuffer_CreateDesc;
class RenderGpuQuery;
class RenderGpuQuery_CreateDesc;
class RenderGpuStorageBuffer;
class RenderGpuStorageBuffer_CreateDesc;
class RenderIndexArray;
class RenderMesh;
class RenderMultiGpuBuffer;
class RenderRequest;
class RenderStageBuffer;
class RenderStageBuffer_CreateDesc;
class RenderStorageBuffer;
class RenderStorageBuffer_CreateDesc;
class RenderState;
class RenderSubMesh;
class RenderSwapChain;
class RenderVertexArray;
class RenderVertexDesc;

enum class RenderCommandType		: u32;
enum class RenderGpuBufferType		: u8;
enum class RenderState_BlendFactor	: u8;
enum class RenderState_BlendOp		: u8;
enum class RenderState_DepthTestOp	: u8;

class SamplerState;
class Shader;
class Shader_CreateDesc;
class ShaderPass;

class VertexLayout;
class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;

class RenderCommonBase {
public:
	using AdapterInfo		= RenderAdapterInfo;

	using BlendOp			= RenderState_BlendOp;
	using BlendFactor		= RenderState_BlendFactor;
	using BuiltInAssets		= RenderBuiltInAssets;

	using DataType			= RenderDataType;
	using DataTypeUtil		= RenderDataTypeUtil;
	using DepthTestOp		= RenderState_DepthTestOp;

	using Fence				= RenderFence;

	using GpuBuffer			= RenderGpuBuffer;

	using Mat4				= Mat4f;
	using Material			= Material;
	using MaterialPass		= MaterialPass;
	using Mesh				= RenderMesh;
	using MultiGpuBuffer	= RenderMultiGpuBuffer;

	using PrimitiveType 	= RenderPrimitiveType;

	using Rect2				= Rect2f;

	using SubMesh			= RenderSubMesh;
	using SwapChain			= RenderSwapChain;

	using Texture			= Texture;
	using Texture1D			= Texture1D;
	using Texture2D			= Texture2D;
	using Texture3D			= Texture3D;
	using TextureCube		= TextureCube;

	using Vec2				= Vec2f;
	using Vec3				= Vec3f;
	using Vec4				= Vec4f;
}; // RenderCommonBase
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(RenderCommonBase);

class RenderAdapterInfo {
public:
	String	adapterName;
	i64		memorySize	= 0;
	i64		LUID		= 0;
}; // RenderAdapterInfo

} // namespace axe
