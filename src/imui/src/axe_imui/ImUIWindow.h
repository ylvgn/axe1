#pragma once

#include "ImUIView.h"

namespace axe {

class ImUIRenderView;
class ImUIWindow : public ImUIView {
	using Thist =  ImUIWindow;
	using Base  = ImUIView;
public:
	ImUIWindow();
	~ImUIWindow();

	void setWindowTitle(StrView title);

	RenderContext*	renderContext() { return _contentView ? _contentView->renderContext() : nullptr; }
	ImUIRenderView* contentView()	{ return _contentView; }

	void render();

protected:
	virtual void onWindowCloseButton() {}
	virtual void onSetSize(const Vec2f& s) override;

	virtual void onNativeWorldPosChanged(const Vec2f&  pos);
	virtual void onNativeSizeChanged	(const Vec2f& size);

	virtual void onUIMouseEvent	(UIMouseEvent& ev)		{}
	virtual void onUIKeyEvent	(UIKeyEvent&   ev)		{}

private:
	class NativeWin;
	UPtr<NativeWin>		 _nativeWin;

	SPtr<ImUIRenderView> _contentView;
}; // ImUIWindow

} // namespace axe