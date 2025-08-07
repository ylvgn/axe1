#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderContext_EventHandler {
public:
	void render(RenderContext* ctx);

	virtual void onRender(RenderContext& ctx) {};
}; // RenderContext_EventHandler


class RenderContext_CreateDesc {
public:
	using EventHandler = RenderContext_EventHandler;

//	NativeUIWindow::CreateDesc	winDesc; no need atm
	NativeUIWindow*				window		 = nullptr;
	EventHandler*				eventHandler = nullptr;
}; // RenderContext_CreateDesc


class RenderContext : public RenderDeviceObject {
	AXE_ABSTRACT_CLASS_TYPE(RenderContext, RenderDeviceObject)
	using Vec2  = Vec2f;
	using Rect2 = Rect2f;
public:
	using CreateDesc	= RenderContext_CreateDesc;
	using EventHandler	= CreateDesc::EventHandler;

	NativeUIWindow*	  window()		 { return _window; }
	EventHandler*	  eventHandler() { return _eventHandler; }

			void beginRender();
	virtual void onBeginRender() {}

			void endRender();
	virtual void onEndRender() {}

			void setSwapChainFrameBufferSize(const Vec2& newSize);
	virtual void onSetSwapChainFrameBufferSize(const Vec2& newSize) { _swapChainFrameBufferSize = newSize; }
	const auto& swapChainFrameBufferSize() const { return _swapChainFrameBufferSize; }

//	void setNativeViewRect(const Rect2& rect); no need atm
//	virtual void onSetNativeViewRect(const Rect2& rect) {}; no need atm
			void commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }
	virtual void onCommit(RenderCommandBuffer& cmdBuf) = 0;

//	void setNeedToRender(); no need atm
	//	virtual void onSetNeedToRender() = 0; no need atm
protected:
	RenderContext(RenderDevice* device, CreateDesc& desc) noexcept; // please create from 'RenderDevice::createRenderContext'

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

	Vec2			_swapChainFrameBufferSize{ 0, 0 };
	NativeUIWindow* _window = nullptr;
	EventHandler*	_eventHandler = nullptr;
}; // RenderContext

} // namespace axe