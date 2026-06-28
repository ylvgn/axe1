#pragma once

#include "RenderDataType.h"

namespace axe {

class EditMesh;

class MaterialPass;

enum class MonitorDisplayMode		: u8;
enum class RenderCommandType		: u32;
enum class RenderGpuBufferType		: u8;
enum class RenderState_BlendFactor	: u8;
enum class RenderState_BlendOp		: u8;
enum class RenderState_DepthTestOp	: u8;

AXE_RenderObject_ForwardDeclare(Material)
AXE_RenderObject_ForwardDeclare(Renderer)

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
AXE_RenderObject_ForwardDeclare(RenderContext)
class RenderDataTypeUtil;
class RenderDeviceObject;
AXE_RenderObject_ForwardDeclare(RenderDevice)
class RenderFence;
AXE_RenderObject_ForwardDeclare(RenderGpuBuffer)
AXE_RenderObject_ForwardDeclare(RenderGpuQuery)
AXE_RenderObject_ForwardDeclare(RenderGpuStorageBuffer)
class RenderIndexArray;
class RenderMesh;
class RenderMultiGpuBuffer;
class RenderRequest;
class RenderStageBuffer;
class RenderStageBuffer_CreateDesc;
AXE_RenderObject_ForwardDeclare(RenderStorageBuffer)
class RenderState;
class RenderSubMesh;
class RenderSwapChain;
class RenderVertexArray;
class RenderVertexDesc;

class SamplerState;
AXE_RenderObject_ForwardDeclare(Shader)
class ShaderPass;

class Texture;
class Texture1D;
class Texture2D;
class Texture3D;
class TextureCube;

class VertexLayout;

class RenderAdapterInfo {
public:
	String	name;
	Int		memorySize	= 0;
}; // RenderAdapterInfo

} // namespace axe
