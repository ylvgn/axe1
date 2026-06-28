#pragma once

#include "RenderCommonBase.h"

namespace axe {

class RenderCapabilities_Info {
public:
	bool hasTearing		  = false; // TODO, not GPU capability, just swapchain behavior.
	bool hasComputeShader = false;
	bool shaderHasFloat64 = false;
	bool hasRaytracing	  = false;
	bool hasMeshShader	  = false;
}; // RenderCapabilities_Info


class RenderCapabilities : public RenderObject {
	AXE_RTTI_INFO(RenderCapabilities, RenderObject)
public:
	using Info = RenderCapabilities_Info;

	bool hasTearing()		const { return _info.hasTearing; } // TODO, will remove somewhere
	bool hasComputeShader() const { return _info.hasComputeShader; }
	bool shaderHasFloat64() const { return _info.shaderHasFloat64; }
	bool hasRaytracing()	const { return _info.hasRaytracing; }
	bool hasMeshShader()	const { return _info.hasMeshShader; }

protected:
	RenderCapabilities() = default;

	Info _info;
}; // RenderCapabilities

} // namespace axe