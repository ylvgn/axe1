#include <axe_engine.h>

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

class EditorApp_CreateDesc;

class EditorApp : public NativeUIApp {
	using This = EditorApp;
	using Base = NativeUIApp;
public:
	EditorApp();
	virtual ~EditorApp() override;
	
	static EditorApp* s_instance();
	virtual void onCreate() final;
private:
	UPtr<EditorMainWindow> _mainWin;
	RenderDevice*		   _renderDevice = nullptr;
}; // EditorApp

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;
	}
};

} // namespace axe