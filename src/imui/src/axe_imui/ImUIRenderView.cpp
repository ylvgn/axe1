#include "ImUIRenderView.h"

namespace axe {

void ImUIRenderView::createRenderContext(RenderContext::CreateDesc& desc) {
	_renderContext = RenderContext::s_new(desc);
}

void ImUIRenderView::onSizeChanged(const Vec2f& size) {
	_renderContext->setSwapChainFrameBufferSize(size);
}

void ImUIRenderView::render() {
	if (_renderContext && _renderContext->eventHandler()) {
		_renderContext->eventHandler()->render(_renderContext.get());
	}
}

} // namespace axe

