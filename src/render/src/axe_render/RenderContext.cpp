#include "RenderContext.h"
#include "Renderer.h"

namespace axe {

const TypeInfo* RenderContext::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderContext";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

void RenderContext_EventHandler::render(RenderContext* ctx) {
	onRender(*ctx);
}

RenderContext::RenderContext(RenderDevice* device, CreateDesc& desc) noexcept
	: Base(device)
	, _window(desc.window)
	, _eventHandler(desc.eventHandler)
{
}

void RenderContext::beginRender() {
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
}

void RenderContext::setSwapChainFrameBufferSize(const Vec2& newSize) {
	if (_swapChainFrameBufferSize == newSize)
		return;
	onSetSwapChainFrameBufferSize(newSize);
}

} // namespace axe