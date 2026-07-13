#include "EditorApp.h"
#include "EditorMainWindow.h"

namespace axe {

class EditorApp_CreateDesc : public EditorApp::CreateDesc {
public:
	EditorApp_CreateDesc() {
		peekMessage = true;

#if true // just for test
		rendererCreateDesc.api = RendererApi::Dx12;
#else
		rendererCreateDesc.api = RendererApi::Vk;
#endif
	}
};


EditorApp::EditorApp()
	: Base(EditorApp_CreateDesc())
{
	setCurDirRelativeToExecutable("/../../../Test101");
}

EditorApp::~EditorApp() {
}

void EditorApp::onCreate() {
	Base::onCreate();

	auto curDir = Directory::current();
	AXE_LOG("CurrentDir = \"{}\"", curDir);

	_mainWin = UPtr_make<EditorMainWindow>();
}

void EditorApp::onPeekMessage() {
	if (_mainWin) {
		_mainWin->render();
	}
}

} // namespace axe
