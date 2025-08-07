#include <axe_editor.h>

namespace axe {

class EditorApp;
class EditorMainWindow;

class EditorMainWindow : public NativeUIWindow {
	using This = EditorMainWindow;
	using Base = NativeUIWindow;
public:
	EditorMainWindow();

	static CreateDesc& s_createDesc();

	virtual void onCreate(CreateDesc& desc) final;
	virtual void onCloseButton() final;
	virtual void onDraw() final;

private:
	UPtr<RenderDevice>	_renderDevice;
	SPtr<RenderContext> _renderContext;

	class EventHandler : public RenderContext::EventHandler {
	public:
		virtual void onRender(RenderContext& renderContext) final;

		void test_mesh()
		{
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

		RenderCommandBuffer _cmdBuf;
		RenderMesh			_renderMesh;
	};
	EventHandler _eventHandler;
}; // EditorMainWindow

class EditorApp : public NativeUIApp {
	using This = EditorApp;
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) final;
private:
	UPtr<EditorMainWindow> _mainWin;
	RenderDevice*		   _renderDevice = nullptr;
}; // EditorApp


void EditorApp::onCreate(CreateDesc& desc) {
	setCurDirRelativeToExecutable("/../../../Test101");

	{ // create renderer
		Renderer::CreateDesc renderDesc;
		renderDesc.api = RendererApi::DX12;
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
	NativeUIApp::current()->quit(0);
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

	renderContext.beginRender();

	_cmdBuf.reset(&renderContext);
	_cmdBuf.clearFrameBuffers()->setColor({ 0, 0, 0.2f, 1 });
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

} // namespace axe

AXE_MAIN(axe::EditorApp)