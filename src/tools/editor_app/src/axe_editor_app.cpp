#include <axe_editor.h>

namespace axe {

class MainWin : public NativeUIWindow {
	using Base = NativeUIWindow;
public:
	virtual void onCreate(CreateDesc& desc) override {
		Base::onCreate(desc);
	}

	virtual void onCloseButton() override {
		NativeUIApp::current()->quit(0);
	}
};

class EditorApp : public NativeUIApp {
	using Base = NativeUIApp;
public:
	virtual void onCreate(CreateDesc& desc) override {
		setCurDirRelativeToExecutable("/../../../Test101");

		AXE_ZoneScopedN("Foo Memory leak");

		int* a = new int[8];
		a[0]   = 100;
//		delete[] a;

		{ // create window
			TempString title = "AXE Editor";

			NativeUIWindow::CreateDesc winDesc;
			winDesc.isMainWindow = true;
			_mainWin.create(winDesc);
			_mainWin.setWindowTitle(title);
		}
	}

private:
	MainWin	_mainWin;
};

} // namespace axe

AXE_MAIN(axe::EditorApp)