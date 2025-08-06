#pragma once

#include "RenderCommonBase.h"

namespace axe {

class RenderDeviceObject : public Object, public RenderCommonBase {
	AXE_ABSTRACT_CLASS_TYPE(RenderDeviceObject, Object)
public:
	RenderDeviceObject() = default;
	explicit RenderDeviceObject(RenderDevice* device) noexcept;

	void _internalSetDevice(RenderDevice* device);

	RenderDevice* device() const;

protected:
	RenderDevice* _device = nullptr;
}; // RenderDeviceObject

} // namespace axe