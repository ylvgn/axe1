#include "ImUIApp.h"

namespace axe {

void ImUIApp::onCreate() {
	Base::onCreate();

	_renderer.reset(Renderer::s_create(_renderSystemDesc));

	{ // create render device (root)
		_renderDevice.reset(_renderer->createRenderDevice(_renderSystemDesc));
		AXE_ASSERT(_renderer->devices().size() > 0);
	}
}

} // namespace axe
