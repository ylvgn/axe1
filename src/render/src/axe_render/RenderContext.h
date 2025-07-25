#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderContext_CreateDesc {
public:
	NativeUIWindow* window = nullptr;
}; // RenderContext_CreateDesc


class RenderContext : public RenderDeviceObject {
	using This = RenderContext;
	using Base = RenderDeviceObject;
public:
	using CreateDesc = RenderContext_CreateDesc;

	virtual ~RenderContext() = default;

	void beginRender();
	void endRender();

	void setFrameBufferSize(Vec2f newSize);
	const Vec2f& frameBufferSize() const { return _frameBufferSize; }

	void commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }

protected:
	RenderContext(RenderDevice* device, CreateDesc& desc) noexcept; // please create from 'RenderDevice::createRenderContext'

	virtual void onBeginRender() {}
	virtual void onEndRender()	 {}

	virtual void onSetFrameBufferSize(const Vec2f& newSize) {};
	virtual void onCommit(RenderCommandBuffer& cmdBuf) {}

	template<class IMPL>
	void _dispatch(IMPL* impl, RenderCommandBuffer& cmdBuf) {
		using Cmd = RenderCommandType;

		#define AXE_MACRO_OP(E) \
			case Cmd::E: { \
				auto* c = static_cast<RenderCommand_##E*>(cmd); \
				impl->onCmd_##E(*c); \
			} break; \
		//----

		for (auto* cmd : cmdBuf.commands()) {
			switch (cmd->type()) {
				AXE_MACRO_OP(ClearFrameBuffers)
//				AXE_MACRO_OP(SetViewport)
//				AXE_MACRO_OP(SwapBuffers)
//				AXE_MACRO_OP(DrawCall)
//				AXE_MACRO_OP(SetScissorRect)
				default: AXE_THROW();
			}
		}
		#undef AXE_MACRO_OP
	}

	Vec2f _frameBufferSize {0,0};
}; // RenderContext

} // namespace axe