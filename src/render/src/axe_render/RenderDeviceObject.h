#pragma once

#include "RenderCommonBase.h"

namespace axe {

class RenderDeviceObject : public Object, public RenderCommonBase {
	AXE_ABSTRACT_CLASS_TYPE(RenderDeviceObject, Object)
public:
	void _internalSetDevice(RenderDevice* device);

	RenderDevice* device() const;

protected:
	RenderDeviceObject() = default;
	RenderDeviceObject(RenderDevice* device) noexcept;

	RenderDevice* _device = nullptr;
}; // RenderDeviceObject

} // namespace axe