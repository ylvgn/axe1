#pragma once

#include "RenderDataType.h"
#include "Render_Common.h"

namespace axe {

class RenderObject_CreateDesc {
public:
	StrView debugName;
}; // RenderObject_CreateDesc


class RenderObject : public Object, public RenderCommonBase {
	using This = RenderObject;
	using Base = Object;
public:
	using CreateDesc = RenderObject_CreateDesc;
protected:
	RenderObject(CreateDesc& desc) noexcept;
}; // RenderObject

} // namespace axe