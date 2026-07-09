#include "ImUIRenderView.h"
#include <axe_render/RenderContext.h>

namespace axe {

void ImUIRenderView::createRenderContext(RenderContext::CreateDesc& desc) {
	_renderContext = RenderContext::s_new(desc);
}

void ImUIRenderView::onSizeChanged(const Vec2f& size) {
	Base::onSizeChanged(size);
	if (_renderContext) {
		_renderContext->setSwapChainFrameBufferSize(Vec2i::s_cast(size));
	}
}

void ImUIRenderView::render() {
	if (_renderContext) {
		_renderContext->render();
	}
}

} // namespace axe

