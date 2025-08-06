#include "RenderDeviceObject.h"
#include "RenderDevice.h"

namespace axe {

const TypeInfo* RenderDeviceObject::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderDeviceObject";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

RenderDeviceObject::RenderDeviceObject(RenderDevice* device) noexcept
	: _device(device)
{
}

void RenderDeviceObject::_internalSetDevice(RenderDevice* device) {
	_device = device;
}

RenderDevice* RenderDeviceObject::device() const {
	return _device;
}

} // namespace axe

