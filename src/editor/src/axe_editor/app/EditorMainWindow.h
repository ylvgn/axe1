#pragma once
#include "EditorWindow.h"

namespace axe {

class EditorMainWindow : public EditorWindow {
	AXE_RTTI_INFO(EditorMainWindow, EditorWindow)
public:
	EditorMainWindow();

	virtual void onWindowCloseButton() final;

private:
	class EventHandler : public RenderContext::EventHandler {
	public:
		virtual void onRender(RenderContext& renderContext) final;

		void test_mesh();

		RenderCommandBuffer _cmdBuf;
		RenderMesh			_renderMesh;
	};
	EventHandler _eventHandler;

}; // EditorMainWindow

} // namespace axe