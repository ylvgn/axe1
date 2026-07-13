#if AXE_RENDER_HAS_VK

#include "Device_Vk.h"
#include "Renderer_Vk.h"
#include "RenderContext_Vk.h"
#include "Capabilities_Vk.h"

namespace axe {

Device_Vk::Device_Vk(CreateDesc& desc)
	: Base(desc)
	, _phyDev(nullptr)
{
	auto* renderer			 = Renderer_Vk::s_instance();
	auto& physicalDeviceList = renderer->physicalDeviceList();
	
	for (auto& phyDev : physicalDeviceList.devices()) {
		auto& props = phyDev.props();
		auto& baseProps = props.base.properties;
		if (desc.adapterInfo) {
			auto deviceName = TempString::s_make(baseProps.deviceName);
			if (desc.adapterInfo->name == deviceName) {
				_phyDev = &phyDev;
				break;
			}
		}
		if (baseProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			_phyDev = &phyDev; // fallback
		}
	}
	if (!_phyDev) {
		_phyDev = physicalDeviceList.defaultDevice();
	}
	AXE_ASSERT(_phyDev != nullptr);
	
	_capabilities = new Capabilities_Vk(*_phyDev);
	_device.create(*this);
}

} // namespace axe

#endif // AXE_RENDER_HAS_VK