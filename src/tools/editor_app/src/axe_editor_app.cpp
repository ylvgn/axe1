#include <axe_editor.h>

namespace axe {

class EditorApp;

class MainWin : public NativeUIWindow
{
	using Base = NativeUIWindow;
public:

	EditorApp* app();

	virtual void onCreate(CreateDesc& desc) override;

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}

	virtual void onDraw() override {
		if (!_renderContext)
			return;

		_renderContext->setFrameBufferSize(clientRect().size);
		_renderContext->beginRender();
		_renderContext->endRender();

		onDrawNeeded();
	}

	SPtr<RenderContext> _renderContext;
};

class EditorApp : public NativeUIApp {
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
				FmtTo(title, " [{}({}, VSync: {})]", device->debugName(), device->api(), device->vsync());
			}

			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			_mainWin = new MainWin();
			_mainWin->create(winDesc);
			_mainWin->setWindowTitle(title);
		}
	}

private:
	MainWin*		_mainWin;
	RenderDevice*	_renderDevice;
};


EditorApp* MainWin::app() {
	return static_cast<EditorApp*>(NativeUIApp::current());
}

void MainWin::onCreate(CreateDesc& desc)
{
	Base::onCreate(desc);

	{ // create render context
		RenderContext::CreateDesc renderContextDesc;
		renderContextDesc.window = this;
		_renderContext = app()->renderDevice()->createContext(renderContextDesc);
	}
}


} // namespace axe

AXE_MAIN(axe::EditorApp)