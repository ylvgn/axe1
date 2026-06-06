#include "ImUIApp.h"

namespace axe {

void ImUIApp::onCreate() {
	Base::onCreate();

	_renderer.reset(Renderer::s_create(_rendererCreateDesc));

	{ // create render device (root)
		_renderDevice.reset(_renderer->createRenderDevice(_renderDeviceCreateDesc));
		AXE_ASSERT(_renderer->devices().size() > 0);
	}
}

} // namespace axe
