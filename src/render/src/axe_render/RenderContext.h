#pragma once

#include "RenderDeviceObject.h"

namespace axe {

AXE_VC_WARNING_PUSH()
AXE_VC_WARNING_DISABLE(5204) // warning C5204: class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
class RenderContext_EventHandler {
public:
	void render(RenderContext* ctx);

	virtual void onRender(RenderContext& ctx) {}
}; // RenderContext_EventHandler
AXE_VC_WARNING_POP()

class RenderContext_CreateDesc {
public:
	NativeUIWindow*	window = nullptr;
	NativeUIWindow::CreateDesc windowDesc;
//	SwapChainDesc              swapChainDesc;
}; // RenderContext_CreateDesc


class RenderContext : public RenderDeviceObject {
	AXE_RTTI_INFO(RenderContext, RenderDeviceObject)
public:
	using CreateDesc	= RenderContext_CreateDesc;
	using EventHandler	= RenderContext_EventHandler;

	static UPtr<This> s_new(/*const MemAllocRequest& req,*/ CreateDesc& desc, int deviceIndex = 0);

			void   beginRender();
	virtual void onBeginRender() {}

			void   endRender();
	virtual void onEndRender() {}

			void   setSwapChainFrameBufferSize(const Vec2i& newSize);
	virtual void onSetSwapChainFrameBufferSize(const Vec2i& newSize) { _swapChainFrameBufferSize = newSize; }
	const auto& swapChainFrameBufferSize() const { return _swapChainFrameBufferSize; }

			void   commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }
	virtual void onCommit(RenderCommandBuffer& cmdBuf) = 0;

	NativeUIWindow*				window()		const { return _window; }

	void						setEventHandler(EventHandler* eventHandler) { _eventHandler = eventHandler; }
	RenderContext_EventHandler* eventHandler() const { return _eventHandler; }

	void render();
	
#if AXE_OS_WINDOWS
	::HWND						hwnd()	const { return _window->hwnd(); }
#endif

protected:
	RenderContext(RenderDevice& device, const CreateDesc& desc) noexcept; // please create from 'Renderer::newRenderContext'

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
				AXE_MACRO_OP(SetViewport)
				AXE_MACRO_OP(SetScissorRect)
				AXE_MACRO_OP(ClearFrameBuffers)
				AXE_MACRO_OP(SwapBuffers)
				AXE_MACRO_OP(DrawCall)
				default: AXE_THROW;
			}
		}
		#undef AXE_MACRO_OP
	}

	Vec2i			_swapChainFrameBufferSize {0,0};
	NativeUIWindow* _window			= nullptr;
	EventHandler*	_eventHandler	= nullptr;
}; // RenderContext

} // namespace axe