#include "RenderContext.h"
#include "RenderDevice.h"

namespace axe {

UPtr<RenderContext> RenderContext::s_new(/*const MemAllocRequest& req,*/ CreateDesc& desc, int deviceIndex /*= 0*/) {
	return Renderer::s_instance()->newRenderContext(desc, deviceIndex);
}

void RenderContext_EventHandler::render(RenderContext* ctx) {
	onRender(*ctx);
}

RenderContext::RenderContext(RenderDevice& device, const CreateDesc& desc) noexcept
	: Base(device)
	, _window(desc.window)
{
}

void RenderContext::render() {
	if (_eventHandler) {
		_eventHandler->render(this);
	}
}

void RenderContext::beginRender() {
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
}

void RenderContext::setSwapChainFrameBufferSize(const Vec2i& newSize) {
	if (_swapChainFrameBufferSize == newSize)
		return;
	onSetSwapChainFrameBufferSize(newSize);
}

} // namespace axe