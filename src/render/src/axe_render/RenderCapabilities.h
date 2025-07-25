#pragma once

#include "RenderCommonBase.h"

namespace axe {

class RenderCapabilities_Info {
public:
	bool hasTearing		  = false;
	bool hasComputeShader = false;
	bool shaderHasFloat64 = false;
	bool hasRaytracing	  = false;
	bool hasMeshShader	  = false;
}; // RenderCapabilities_Info


class RenderCapabilities : public NonCopyable {
public:
	using Info = RenderCapabilities_Info;

	bool hasTearing()		const { return _info.hasTearing; }
	bool hasComputeShader() const { return _info.hasComputeShader; }
	bool shaderHasFloat64() const { return _info.shaderHasFloat64; }
	bool hasRaytracing()	const { return _info.hasRaytracing; }
	bool hasMeshShader()	const { return _info.hasMeshShader; }

protected:
	RenderCapabilities() = default;

	Info _info;
}; // RenderCapabilities

} // namespace axe