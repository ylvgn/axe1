#include "RenderContext.h"
#include "RenderDevice.h"

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

UPtr<RenderContext> RenderContext::s_new(CreateDesc& desc, int deviceIndex /*= 0*/) {
	return Renderer::s_instance()->newRenderContext(desc, deviceIndex);
}

void RenderContext_EventHandler::render(RenderContext* ctx) {
	onRender(*ctx);
}

RenderContext::RenderContext(RenderDevice* device, const CreateDesc& desc) noexcept
	: Base(device)
	, _window(desc.window)
{
}

void RenderContext::beginRender() {
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
}

void RenderContext::setSwapChainFrameBufferSize(const Vec2f& newSize) {
	if (_swapChainFrameBufferSize == newSize)
		return;
	onSetSwapChainFrameBufferSize(newSize);
}

} // namespace axe