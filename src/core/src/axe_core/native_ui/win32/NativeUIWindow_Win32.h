#pragma once

#include "../base/NativeUIWindow_Base.h"

#if AXE_OS_WINDOWS

namespace axe {

class Win32Util;

class NativeUIWindow_Win32 : public NativeUIWindow_Base {
	using This = NativeUIWindow_Win32;
	using Base = NativeUIWindow_Base;
	using Util = Win32Util;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onSetNativeWindowTitle(StrView title) override;
	virtual void onSetNativeWorldPos(const Vec2f& screenPos) override;
	virtual void onSetNativeSize(const Vec2f& size) override;
	virtual void onSetNativeCursor(UIMouseCursor type) override;
	virtual void onDrawNeeded() override;

public:
	bool isKeyDown(KeyCode keyCode);

	HWND _hwnd = nullptr;

private:
	static LRESULT WINAPI s_wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static Rect2 s_win32_getWorldRect(HWND hwnd);

	AXE_INLINE static This* s_getThis(HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	LRESULT _handleNativeEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool _handleNativeUIKeyboardEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void _resetModifiedKeyCodeState(UIKeyCodeEventType& keyState);

	bool _isKeyDown(int vkKeyCode) { return ::GetKeyState(vkKeyCode) & 0x8000; }

	UIEventModifier _getWin32Modifier();
};

} // namespace axe

#endif // AXE_OS_WINDOWS