#pragma once
#include <axe_engine.h> // TODO may move to module common

namespace axe {

class EditorMainWindow;

class EditorApp : public ImUIApp {
	AXE_RTTI_INFO(EditorApp, ImUIApp)
public:
	AXE_DOWNCAST_GET_INSTANCE()

	EditorApp();
	virtual ~EditorApp() override;
	
	virtual void onCreate() final;
private:

	virtual void onPeekMessage() override;

	UPtr<EditorMainWindow> _mainWin;
}; // EditorApp

} // namespace axe