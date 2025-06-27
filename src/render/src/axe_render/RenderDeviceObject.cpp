#include "RenderDeviceObject.h"
#include "RenderDevice.h"

namespace axe {

RenderDeviceObject::RenderDeviceObject(CreateDesc& desc) noexcept
	: Base(desc)
	, _device(desc.device)
{
	AXE_ASSERT(desc.device != nullptr);
}

} // namespace axe

