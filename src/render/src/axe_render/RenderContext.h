#pragma once

#include "RenderDeviceObject.h"

namespace axe {

AXE_VC_WARNING_PUSH()
AXE_VC_WARNING_DISABLE(5204) // warning C5204: class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
class RenderContext_EventHandler {
public:
	void render(RenderContext* ctx);

	virtual void onRender(RenderContext& ctx) {};
}; // RenderContext_EventHandler
AXE_VC_WARNING_POP()

class RenderContext_CreateDesc {
public:
	NativeUIWindow*	window = nullptr;
}; // RenderContext_CreateDesc


class RenderContext : public RenderDeviceObject {
	using This = RenderContext;
	using Base = RenderDeviceObject;
	AXE_ABSTRACT_CLASS_TYPE(RenderContext, RenderDeviceObject)
public:
	using CreateDesc	= RenderContext_CreateDesc;
	using EventHandler	= RenderContext_EventHandler;

	static UPtr<This> s_new(CreateDesc& desc, int deviceIndex = 0);

			void   beginRender();
	virtual void onBeginRender() {}

			void   endRender();
	virtual void onEndRender() {}

			void   setSwapChainFrameBufferSize(const Vec2f& newSize);
	virtual void onSetSwapChainFrameBufferSize(const Vec2f& newSize) { _swapChainFrameBufferSize = newSize; }
	const auto& swapChainFrameBufferSize() const { return _swapChainFrameBufferSize; }

			void   commit(RenderCommandBuffer& cmdBuf) { onCommit(cmdBuf); }
	virtual void onCommit(RenderCommandBuffer& cmdBuf) = 0;

	NativeUIWindow*		window()		const { return _window; }

	void						setEventHandler(EventHandler* eventHandler) { _eventHandler = eventHandler; }
	RenderContext_EventHandler* eventHandler() const { return _eventHandler; }

#if AXE_OS_WINDOWS // TODO may remove later, and use NativeUIWindow then casting to win impl when use hwnd case
	::HWND				hwnd()			const { return _window->_hwnd; }
#endif

protected:
	RenderContext(RenderDevice* device, const CreateDesc& desc) noexcept; // please create from 'RenderDevice::createRenderContext'

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

	Vec2f			_swapChainFrameBufferSize {0,0};
	NativeUIWindow* _window			= nullptr;
	EventHandler*	_eventHandler	= nullptr;
}; // RenderContext

} // namespace axe