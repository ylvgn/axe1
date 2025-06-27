#include "RenderContext.h"
#include "Renderer.h"

namespace axe {

RenderContext::RenderContext(CreateDesc& desc) noexcept
	: Base(desc)
{
}

void RenderContext::beginRender() {
	onBeginRender();
}

void RenderContext::endRender() {
	onEndRender();
}

void RenderContext::setFrameBufferSize(Vec2f newSize) {
	if (_frameBufferSize == newSize)
		return;

	_frameBufferSize = newSize;
	onSetFrameBufferSize(newSize);
}

} // namespace axe