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

#if 0 // no need atm
void RenderContext::setNeedToRender() {
	onSetNeedToRender();
}
#endif

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

#if 0 // no need atm
void RenderContext::setNativeViewRect(const Rect2& rect_) {
	auto rect = rect_;
	Math::max_it(rect.size, { 4, 4 }); // never let the width/height too small
	if (rect == _nativeViewRect)
		return;
	onSetNativeViewRect(rect);
	_nativeViewRect = rect;
	setNeedToRender();
}
#endif

} // namespace axe