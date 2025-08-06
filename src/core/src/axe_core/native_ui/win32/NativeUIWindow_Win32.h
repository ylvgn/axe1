#pragma once

#include "../base/NativeUIWindow_Base.h"

#if AXE_OS_WINDOWS

namespace axe {

class Win32Util;

class NativeUIWindow_Win32 : public NativeUIWindow_Base {
	using This = NativeUIWindow_Win32;
	using Base = NativeUIWindow_Base;
public:
	using Util = Win32Util;

	LRESULT _handleNativeEvent(::HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool	_handleNativeUIMouseEvent(::HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool	_handleNativeUIKeyboardEvent(::HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static Rect2 s_win32_getWorldRect(::HWND hwnd);

	bool isKeyDown(KeyCode keyCode);

	::HWND _hwnd = nullptr;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onSetNativeWindowTitle(StrView title) override;
	virtual void onSetNativeWorldPos(const Vec2f& screenPos) override;
	virtual void onSetNativeSize(const Vec2f& size) override;
	virtual void onSetNativeCursor(UIMouseCursor type) override;
	virtual void onSetNativeVisible(bool b) override;
	virtual void onDrawNeeded() override;

	virtual void doCloseWindow() override;
	virtual void doDestroyWindow() override;

private:
	static LRESULT WINAPI s_wndProc(::HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	AXE_INLINE static This* s_getThis(::HWND hwnd) {
		return reinterpret_cast<This*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	void _resetModifiedKeyCodeState(UIKeyCodeEventType& keyState);

	bool _isKeyDown(int vkKeyCode) { return ::GetKeyState(vkKeyCode) & 0x8000; }

	UIEventModifier _getWin32Modifier();
}; // NativeUIWindow_Win32

} // namespace axe

#endif // AXE_OS_WINDOWS