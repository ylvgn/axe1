#pragma once

#include "Renderer.h"

namespace axe {

class RenderDeviceObject : public RenderObject {
	AXE_RTTI_INFO(RenderDeviceObject, Object)
public:
	RenderDeviceObject(RenderDevice* device = Renderer::s_rootDevice()) noexcept;

	RenderDevice* device() const;

protected:
	RenderDevice* _device = nullptr;
}; // RenderDeviceObject

} // namespace axe