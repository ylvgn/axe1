#include "RenderDeviceObject.h"

namespace axe {

RenderDeviceObject::RenderDeviceObject(RenderDevice& device) noexcept
	: _device(&device)
{
}

RenderDevice* RenderDeviceObject::device() const {
	return _device;
}

} // namespace axe

