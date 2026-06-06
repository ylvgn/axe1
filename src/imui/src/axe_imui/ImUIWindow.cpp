#include "ImUIWindow.h"
#include "ImUIRenderView.h"

namespace axe {

#if 0
#pragma mark ========= ImUIWindow::NativeWin ============
#endif
class ImUIWindow::NativeWin : public NativeUIWindow {
	using This = NativeWin;
	using Base = NativeUIWindow;
public:
	NativeWin(ImUIWindow* owner) noexcept
		: _owner(owner)
	{}

	virtual void onCloseButton() override {
		_owner->onWindowCloseButton();
	}

	virtual void onSetWorldPos(const Vec2f& pos) override { // aka onWorldPosChanged
		Base::onSetWorldPos(pos);
		_owner->onNativeWorldPosChanged(pos);
	}
	virtual void onSetSize(const Vec2f& size) override { // aka onSizeChanged
		Base::onSetSize(size);
		_owner->onNativeSizeChanged(size);
	}

	virtual void onUIMouseEvent(UIMouseEvent& ev) override {
		Base::onUIMouseEvent(ev);
		_owner->onUIMouseEvent(ev);
	}
	virtual void onUIKeyEvent(UIKeyEvent& ev) override {
		Base::onUIKeyEvent(ev);
		_owner->onUIKeyEvent(ev);
	}

private:
	ImUIWindow* _owner = nullptr;
}; // NativeWin


#if 0
#pragma mark ========= ImUIWindow_CreateDesc ============
#endif
class ImUIWindow_CreateDesc : public NativeUIWindow::CreateDesc {
public:
	ImUIWindow_CreateDesc() noexcept {
		isMainWindow = true;
		visible		 = false; // just keep it false default, and call setVisible(true) when setup
		//rect		 = Rect2f(100, 100, 1920, 1080); // * 0.5f;
	}
}; // ImUIWindow_CreateDesc


#if 0
#pragma mark ========= ImUIWindow ============
#endif
ImUIWindow::ImUIWindow() {
	{ // create native window
		ImUIWindow_CreateDesc desc;
		_nativeWin = UPtr<NativeWin>(new NativeWin(this));
		_nativeWin->create(desc);
	}

	{ // create render context
		_contentView = SPtr_make<ImUIRenderView>();
		addChild(_contentView.ptr());

		RenderContext::CreateDesc desc;
		desc.window = _nativeWin.get();
		_contentView->createRenderContext(desc);
	}

	_nativeWin->setVisible(true); // !<-- set active after render context created  
}

ImUIWindow::~ImUIWindow() {
}

void ImUIWindow::setWindowTitle(StrView title) {
	_nativeWin->setWindowTitle(title);
}

void ImUIWindow::render() {
	if (_contentView) { _contentView->render(); }
}

void ImUIWindow::onNativeWorldPosChanged(const Vec2f& pos) {
	Base::onWorldPosChanged(pos);
}

void ImUIWindow::onNativeSizeChanged(const Vec2f& s) {
	Base::onSizeChanged(s);

	if (_nativeWin) {
		auto rc = _nativeWin->clientRect(); // aka _nativeWin->contentWorldRect();
		if (_contentView) {
			_contentView->setWorldRect(rc);
		}
	}
}

void ImUIWindow::onSetSize(const Vec2f& s) {
	Base::onSetSize(s);

	if (_nativeWin) {
		_nativeWin->setSize(s);
	}
}

} // namespace axe