#pragma once

#include "RenderDataType.h"

namespace axe {

class RenderDataTypeUtil;

class Material;
class MaterialPass;

class Shader;
class ShaderPass;

class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;
class SamplerState;

class Renderer;
class Renderer_CreateDesc;
class RenderDeviceObject;
class RenderDeviceObject_CreateDesc;
class RenderDevice;
class RenderDevice_CreateDesc;
class RenderCapabilities;
class RenderContext;
class RenderContext_CreateDesc;
class RenderSwapChain;
class RenderSwapChain_CreateDesc;
class RenderBuiltInAssets;

class RenderFence;
class RenderFence_CreateDesc;

class RenderGpuQuery;
class RenderGpuQuery_CreateDesc;
class RenderGpuBuffer;
class RenderGpuBuffer_CreateDesc;
class RenderGpuStorageBuffer;
class RenderGpuStorageBuffer_CreateDesc;
class RenderMesh;
class RenderMultiGpuBuffer;
class RenderRequest;

class RenderStageBuffer;
class RenderStageBuffer_CreateDesc;
class RenderStorageBuffer;
class RenderStorageBuffer_CreateDesc;

class RenderState;
class RenderSubMesh;

class RenderVertexArray;
class RenderVertexDesc;
class RenderIndexArray;

class RenderState_BlendOp;
class RenderState_BlendFactor;
class RenderState_DepthTestOp;
class RenderAdapterInfo;

class RenderCommonBase {
public:
	using AdapterInfo		= RenderAdapterInfo;

	using BlendOp			= RenderState_BlendOp;
	using BlendFactor		= RenderState_BlendFactor;
	using BuiltInAssets		= RenderBuiltInAssets;

	using ColorType			= ColorType;

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
	using RenderContext		= RenderContext;
	using RenderState		= RenderState;

	using SamplerState		= SamplerState;
	using Shader			= Shader;
	using ShaderPass		= ShaderPass;
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
	i64		memorySize	 = 0;
	int	    adapterIndex = 0;
}; // RenderAdapterInfo

} // namespace axe
