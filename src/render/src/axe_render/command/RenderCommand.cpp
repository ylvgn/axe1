#include "RenderCommand.h"
#include "../RenderContext.h"

namespace axe {

void RenderCommandBuffer::reset(RenderContext* ctx) {
	for (auto* cmd : _commands) {
		cmd->~RenderCommand();
	}
	_allocator.clear();
	_commands.clear();

	_scissorRect = Rect2f(Vec2f::s_zero(), ctx->swapchainFrameBufferSize());
}

} // namespace axe