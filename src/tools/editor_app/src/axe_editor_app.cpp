#include <axe_editor.h>

namespace axe {

class EditorApp;
class MainWin;

class MainWin : public NativeUIWindow {
	using This = MainWin;
	using Base = NativeUIWindow;
public:
	using VertexT = VertexT_Color<Color4f, 1, Vertex_Pos>;

	virtual void onCreate(CreateDesc& desc) final;
	virtual void onCloseButton() final;
	virtual void onDraw() final;

	EditorApp* app();
	RenderDevice* renderDevice();

	SPtr<RenderContext> _renderContext;
	RenderMesh			_renderMesh;
	RenderCommandBuffer _cmdBuf;
}; // MainWin


class EditorApp : public NativeUIApp {
	using This = EditorApp;
	using Base = NativeUIApp;
public:

	RenderDevice* renderDevice() { return _renderDevice; }

	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test101");

		{ // create renderer
			Renderer::CreateDesc renderDesc;
			renderDesc.api = RendererApi::DX12;
			auto* p = Renderer::s_create(renderDesc);
			
			auto deviceDesc = RenderDevice_CreateDesc();
			_renderDevice	= p->createRenderDevice(deviceDesc);
		}

		{ // create window
			TempString title("AXE Editor -");
			AXE_ASSERT(Renderer::s_instance()->devices().size() > 0);

			for (auto& device : Renderer::s_instance()->devices()) {
				FmtTo(title, " [{}({}, VSync: {})]", device->debugName(), device->api(), device->VSync());
			}

			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			_mainWin = new MainWin();
			_mainWin->create(winDesc);
			_mainWin->setWindowTitle(title);
		}
	}

private:
	MainWin*		_mainWin = nullptr;
	RenderDevice*	_renderDevice = nullptr;
}; // EditorApp


EditorApp* MainWin::app() {
	return static_cast<EditorApp*>(NativeUIApp::current());
}

RenderDevice* MainWin::renderDevice() {
	return app()->renderDevice();
}

void MainWin::onCloseButton() {
	NativeUIApp::current()->quit(0);
}

void MainWin::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);

	{ // create render context
		RenderContext::CreateDesc renderContextDesc;
		renderContextDesc.window = this;
		_renderContext = renderDevice()->createContext(renderContextDesc);
	}

	{
		EditMesh editMesh;

		editMesh.pos.emplace_back(0.0f,  0.5f, 0.0f);
		editMesh.pos.emplace_back(0.5f, -0.5f, 0.0f);
		editMesh.pos.emplace_back(-0.5f,-0.5f, 0.0f);

		editMesh.color.emplace_back(255, 0, 0, 255);
		editMesh.color.emplace_back(0, 255, 0, 255);
		editMesh.color.emplace_back(0, 0, 255, 255);

		_renderMesh._internalSetDevice(renderDevice());
		_renderMesh.create(editMesh);
	}
}

void MainWin::onDraw()
{
	if (!_renderContext)
		return;

	_renderContext->setFrameBufferSize(clientRect().size);
	_renderContext->beginRender();
	_cmdBuf.reset(_renderContext);
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
	_renderContext->commit(_cmdBuf);
	_renderContext->endRender();

	onDrawNeeded();
}

} // namespace axe

AXE_MAIN(axe::EditorApp)