#include "EditorApp.h"

namespace axe {

EditorApp* EditorApp_instance;

EditorApp* EditorApp::s_instance() {
	return EditorApp_instance;
}

EditorApp::EditorApp() : Base(EditorApp_CreateDesc()) {
	AXE_ASSERT(EditorApp_instance == nullptr);
	EditorApp_instance = this;
}

EditorApp::~EditorApp() {
	AXE_ASSERT(EditorApp_instance == this);
	EditorApp_instance = nullptr;
}

void EditorApp::onCreate() {
	Base::onCreate();
	
	setCurDirRelativeToExecutable("/../../../Test101");

	{ // create renderer // TODO use ImGuiRenderView Instead
		Renderer::CreateDesc renderDesc;
		renderDesc.api = RendererApi::Dx12;
		Renderer::s_create(renderDesc);
	}

	{ // create window
		_mainWin = UPtr<EditorMainWindow>(new EditorMainWindow());
		_mainWin->create(EditorMainWindow::s_createDesc());

		_mainWin->setVisible(true);
	}
}

EditorMainWindow::EditorMainWindow() {
	{ // create render device
		auto desc = RenderDevice_CreateDesc();
		_renderDevice.reset(Renderer::s_instance()->createRenderDevice(desc));
		AXE_ASSERT(Renderer::s_instance()->devices().size() > 0);
	}
}

void EditorMainWindow::onCloseButton() {
	//NativeUIApp::current()->quit(0); // TODO
	EditorApp::s_instance()->quit(0);
}

EditorMainWindow::CreateDesc& EditorMainWindow::s_createDesc() {
	static EditorMainWindow::CreateDesc desc;
	desc.isMainWindow = true;
	desc.visible	  = false;
	return desc;
}

void EditorMainWindow::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	AXE_ASSERT(_renderDevice != nullptr);

	{ // create render context
		RenderContext::CreateDesc renderContextDesc;
		renderContextDesc.window	   = this;
		renderContextDesc.eventHandler = &_eventHandler;
		_renderContext				   = _renderDevice->createContext(renderContextDesc);
	}

	{ // set window title
		auto title = TempString::s_format("AXE Editor - [{}({}, VSync: {})]"
										, _renderDevice->debugName()
										, _renderDevice->api()
										, _renderDevice->VSync()
		);
		setWindowTitle(title);
	}
}

void EditorMainWindow::EventHandler::onRender(RenderContext& renderContext) {
	AXE_RUN_ONCE(test_mesh());

	auto& clientRect = renderContext.window()->clientRect();

	renderContext.beginRender();

	_cmdBuf.reset(&renderContext);
	_cmdBuf.setViewport(clientRect);
	_cmdBuf.setScissorRect(clientRect);
	_cmdBuf.clearFrameBuffers()->setColor({ 0.0f, 0.2f, 0.4f, 1.0f });

	{ // draw mesh
		for (auto& sm : _renderMesh.subMeshes())
		{
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

void EditorMainWindow::onDraw()
{
	if (!_renderContext) return;

	AXE_TODO("setSwapChainFrameBufferSize each frame for windows resize, cuz we use single global Native Window client rect");
	_renderContext->setSwapChainFrameBufferSize(clientRect().size);
	_renderContext->eventHandler()->render(_renderContext);
}
}
