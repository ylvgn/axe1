#include "EditorMainWindow.h"
#include "EditorApp.h"

namespace axe {

#if 0
#pragma mark ========= MyTestCase ============
#endif
void EditorMainWindow::EventHandler::test_mesh() {
	AXE_TODO("");
	{
		EditMesh editMesh;

		editMesh.pos.emplace_back(0.0f, 0.5f, 0.0f);
		editMesh.pos.emplace_back(0.5f, -0.5f, 0.0f);
		editMesh.pos.emplace_back(-0.5f, -0.5f, 0.0f);

		editMesh.color.emplace_back(255, 0, 0, 255);
		editMesh.color.emplace_back(0, 255, 0, 255);
		editMesh.color.emplace_back(0, 0, 255, 255);

		_renderMesh.create(editMesh);
	}
}

void EditorMainWindow::EventHandler::onRender(RenderContext& renderContext) {
	AXE_RUN_ONCE(test_mesh());

	auto& clientRect = renderContext.window()->clientRect();

	renderContext.beginRender();

	_cmdBuf.reset(&renderContext);
	_cmdBuf.setViewport(clientRect);
	_cmdBuf.setScissorRect(clientRect);
	_cmdBuf.clearFrameBuffers()->setColor({0.0f, 0.2f, 0.4f, 1.0f});

	{ // draw mesh
		for (auto& sm : _renderMesh.subMeshes()) {
			auto* cmd = _cmdBuf.addDrawCall();
#if _DEBUG
			cmd->debugLoc = SrcLoc();
#endif
			cmd->primitive	  = sm.primitive();
			cmd->vertexLayout = sm.vertexLayout();
			cmd->vertexBuffer = sm.vertexBuffer();
			cmd->vertexCount  = sm.vertexCount();
		}
	}
	_cmdBuf.swapBuffers();
	renderContext.commit(_cmdBuf);

	renderContext.endRender();
}


#if 0
#pragma mark ========= EditorMainWindow ============
#endif
EditorMainWindow::EditorMainWindow() {
	AXE_ASSERT(renderContext() != nullptr);

	auto* renderDevice = renderContext()->device();

	{
		auto title = TempString::s_format("AXE Editor - [{}({}, MT: {}, VSync: {})]",
										  renderDevice->name()
										, renderDevice->api(),
										  renderDevice->multithread()
										, renderDevice->vsync()
		);
		setWindowTitle(title);
	}

	renderContext()->setEventHandler(&_eventHandler);
}

void EditorMainWindow::onWindowCloseButton() {
	EditorApp::s_instance()->quit(0);
}

} // namespace axe