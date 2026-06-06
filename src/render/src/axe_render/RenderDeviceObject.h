#pragma once

#include "Renderer.h"

namespace axe {

class RenderDeviceObject : public Object, public RenderCommonBase {
	AXE_ABSTRACT_CLASS_TYPE(RenderDeviceObject, Object)
public:
	RenderDeviceObject(RenderDevice* device = Renderer::s_rootDevice()) noexcept;

	RenderDevice* device() const;

protected:
	RenderDevice* _device = nullptr;
}; // RenderDeviceObject

} // namespace axe