#pragma once

#include <axe_core/math/Rect2.h>
#include <axe_core/input/UIEvent.h>

namespace axe {

class NativeUIWindow_Base_CreateDesc {
public:
	NativeUIWindow_Base_CreateDesc()
		: titleBar(true)
		, isMainWindow(false)
		, visible(true)
		, resizable(true)
		, closeButton(true)
		, minButton(true)
		, maxButton(true)
		, centerToScreen(true)
		, alwaysOnTop(false)
	{}

	enum class Type {
		None,
		NormalWindow,
		ToolWindow,
		PopupWindow,
	};

	Type		type = Type::NormalWindow;
	Rect2f		rect {10, 10, 640, 480};

	bool		titleBar		: 1;
	bool		isMainWindow	: 1;
	bool		visible			: 1;
	bool		resizable		: 1;
	bool		closeButton		: 1;
	bool		minButton		: 1;
	bool		maxButton		: 1;
	bool		centerToScreen	: 1;
	bool		alwaysOnTop		: 1;

}; // NativeUIWindow_Base_CreateDesc

class NativeUIWindow_Base : public NonCopyable {
	using This = NativeUIWindow_Base;
public:
	using Vec2			= Vec2f;
	using Rect2			= Rect2f;
	using KeyCode		= UIKeyboardEventKeyCode;
	using KeyCodeState	= UIKeyCodeEventType;

	using CreateDesc	= NativeUIWindow_Base_CreateDesc;

	static const int kKeyCodeCount = 256;

			void create	 (CreateDesc& desc)	{ onCreate(desc); }
	virtual void onCreate(CreateDesc& desc) {}

			void setWindowTitle			(StrView title);
	virtual void onSetNativeWindowTitle	(StrView title) = 0;

			void setWorldPos			(const Vec2& pos);
			void setWorldPos			(float x, float y)	{ setWorldPos(Vec2(x, y)); }
	virtual void onSetNativeWorldPos	(const Vec2& pos) = 0;
	virtual void onSetWorldPos			(const Vec2& pos)	{ _worldRect.pos = pos; }

			void setSize				(const Vec2& size);
			void setSize				(float w, float h)	{ setSize(Vec2(w, h)); }
	virtual void onSetSize				(const Vec2& size)	{ _worldRect.size = size; }
	virtual void onSetNativeSize		(const Vec2& size) = 0;

			void setWorldRect			(const Rect2& rect)	{ setWorldPos(rect.pos); setSize(rect.size); }

	virtual void onClientRectChanged(const Rect2f& rc) { _clientRect = rc; }

			void setCursor(UIMouseCursor cursor);
	virtual void onSetNativeCursor(UIMouseCursor type) = 0;

			void		 setVisible(bool b);
	virtual void onSetNativeVisible(bool b) = 0;

	//! same as click close button by user
	void		 closeWindow() { doCloseWindow(); }
	virtual void doCloseWindow() = 0;

			void destroyWindow	() { doDestroyWindow(); }
	virtual void doDestroyWindow() = 0;

			void drawNeeded() { onDrawNeeded(); }
	virtual void onDrawNeeded() {}

	virtual void onCloseButton() {}
	virtual void onActive(bool isActive) {}
	virtual void onDraw() {}

	const Rect2f& worldRect()  const { return _worldRect; }
	const Rect2f& clientRect() const { return _clientRect; }

protected:
	virtual void onUINativeMouseEvent(UIMouseEvent& ev);
	virtual void onUIMouseEvent		 (UIMouseEvent& ev) {}

	virtual void onUINativeKeyboardEvent(UIKeyboardEvent& ev);
	virtual void onUIKeyboardEvent		(UIKeyboardEvent& ev) {}

	Rect2 _worldRect  {0}; //! relative to the screen's top-left corner
	Rect2 _clientRect {0};

	UIMouseEventButton	_pressedMouseButtons = UIMouseEventButton::None;
	Vec2f				_mousePos {0,0};
	
	Vector<UIKeyboardEvent::Type, kKeyCodeCount> _pressedKeyCodes;
}; // NativeUIWindow_Base

} // namespace axe