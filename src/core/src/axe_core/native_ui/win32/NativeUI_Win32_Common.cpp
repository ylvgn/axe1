#if AXE_OS_WINDOWS

#include "NativeUI_Win32_Common.h"
#include <axe_core/input/UIEvent.h>

namespace axe {

void Win32Util::errorTo(String& out, ::DWORD in_errorcode /*= ::WSAGetLastError()*/) {
// retrieving-error-messages: https://docs.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages
// system error code lookup: https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
	out.clear();

	TempStringW tmp;
	tmp.resizeToLocalBufSize();
	auto dwChars = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								   NULL,
								   in_errorcode,
								   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // NULL
								   tmp.data(),
								   static_cast<::DWORD>(tmp.size()),
								   NULL);

	out = dwChars ? UtfUtil::toString(tmp) : Fmt("[{}]", in_errorcode);
}

void Win32Util::convert(Rect2f& o, const ::RECT& i)
{
	o.x = static_cast<float>(i.left);
	o.y = static_cast<float>(i.top);
	o.w = static_cast<float>(i.right - i.left);
	o.h = static_cast<float>(i.bottom - i.top);
}

void Win32Util::convert(Rect2i& o, const ::RECT& i)
{
	o.x = i.left;
	o.y = i.top;
	o.w = i.right - i.left;
	o.h = i.bottom - i.top;
}

void Win32Util::convert(Vec2f& o, const ::POINT& i)
{
	o.x = static_cast<float>(i.x);
	o.y = static_cast<float>(i.y);
}

void Win32Util::convert(Vec2i& o, const ::POINT& i)
{
	o.x = i.x;
	o.y = i.y;
}

void Win32Util::convert(Vec2f& o, const ::SIZE& i)
{
	o.x = static_cast<float>(i.cx);
	o.y = static_cast<float>(i.cy);
}

void Win32Util::convert(Vec2i& o, const ::SIZE& i)
{
	o.x = static_cast<int>(i.cx);
	o.y = static_cast<int>(i.cy);
}

void Win32Util::convert(::RECT& o, const Rect2f& i)
{
	o.left	 = static_cast<LONG>(i.x);
	o.top	 = static_cast<LONG>(i.y);
	o.right	 = static_cast<LONG>(i.xMax());
	o.bottom = static_cast<LONG>(i.yMax());
}

void Win32Util::convert(::POINT& o, const Vec2f& i)
{
	o.x = static_cast<LONG>(i.x);
	o.y = static_cast<LONG>(i.y);
}

void Win32Util::convert(::POINT& o, const Vec2i& i)
{
	o.x = static_cast<LONG>(i.x);
	o.y = static_cast<LONG>(i.y);
}

void Win32Util::convert(::SIZE& o, const Vec2f& i)
{
	o.cx = static_cast<LONG>(i.x);
	o.cy = static_cast<LONG>(i.y);
}

inline void Win32Util::convert(::SIZE& o, const Vec2i& i)
{
	o.cx = static_cast<LONG>(i.x);
	o.cy = static_cast<LONG>(i.y);
}

int Win32Util::toVKKey(const KeyCode& i) {

#define CASE_E(AXE_T, WIN32_VK_T) case KeyCode::AXE_T: return WIN32_VK_T;
	switch (i) {
		CASE_E(Ctrl,			VK_CONTROL)
		CASE_E(LCtrl,			VK_LCONTROL)
		CASE_E(RCtrl,			VK_RCONTROL)
		CASE_E(LShift,			VK_LSHIFT)
		CASE_E(RShift,			VK_RSHIFT)
		CASE_E(Shift,			VK_SHIFT)
		CASE_E(LAlt,			VK_LMENU)
		CASE_E(RAlt,			VK_RMENU)
		CASE_E(Alt,				VK_MENU)
		CASE_E(LCmd,			VK_LWIN)
		CASE_E(RCmd,			VK_RWIN)
		CASE_E(Enter,			VK_RETURN)
		CASE_E(Escape,			VK_ESCAPE)
		CASE_E(Tab,				VK_TAB)
		CASE_E(CapsLock,		VK_CAPITAL)
		CASE_E(Space,			VK_SPACE)
		CASE_E(Backspace,		VK_BACK)
		CASE_E(Semicolon,		VK_OEM_1)		// ;:
		CASE_E(Slash,			VK_OEM_2)		// /?
		CASE_E(BackQuote,		VK_OEM_3)		// `~
		CASE_E(LeftBracket,		VK_OEM_4)		// [{
		CASE_E(Backslash,		VK_OEM_5)		// \|
		CASE_E(RightBracket,	VK_OEM_6)		// ]}
		CASE_E(Quote,			VK_OEM_7)		// '~
		CASE_E(Equals,			VK_OEM_PLUS)	// =+
		CASE_E(Hyphen,			VK_OEM_MINUS)	// -_
		CASE_E(Comma,			VK_OEM_COMMA)	// ,<
		CASE_E(Period,			VK_OEM_PERIOD)	// .>
		CASE_E(F1,				VK_F1)
		CASE_E(F2,				VK_F2)
		CASE_E(F3,				VK_F3)
		CASE_E(F4,				VK_F4)
		CASE_E(F5,				VK_F5)
		CASE_E(F6,				VK_F6)
		CASE_E(F7,				VK_F7)
		CASE_E(F8,				VK_F8)
		CASE_E(F9,				VK_F9)
		CASE_E(F10,				VK_F10)
		CASE_E(F11,				VK_F11)
		CASE_E(F12,				VK_F12)
		CASE_E(Alpha1,			'1') // 1!
		CASE_E(Alpha2,			'2') // 2@
		CASE_E(Alpha3,			'3') // 3#
		CASE_E(Alpha4,			'4') // 4$
		CASE_E(Alpha5,			'5') // 5%
		CASE_E(Alpha6,			'6') // 6^
		CASE_E(Alpha7,			'7') // 7&
		CASE_E(Alpha8,			'8') // 8*
		CASE_E(Alpha9,			'9') // 9(
		CASE_E(Alpha0,			'0') // 0)
		CASE_E(A,				'A')
		CASE_E(B,				'B')
		CASE_E(C,				'C')
		CASE_E(D,				'D')
		CASE_E(E,				'E')
		CASE_E(F,				'F')
		CASE_E(G,				'G')
		CASE_E(H,				'H')
		CASE_E(I,				'I')
		CASE_E(J,				'J')
		CASE_E(K,				'K')
		CASE_E(L,				'L')
		CASE_E(M,				'M')
		CASE_E(N,				'N')
		CASE_E(O,				'O')
		CASE_E(P,				'P')
		CASE_E(Q,				'Q')
		CASE_E(R,				'R')
		CASE_E(S,				'S')
		CASE_E(T,				'T')
		CASE_E(U,				'U')
		CASE_E(V,				'V')
		CASE_E(W,				'W')
		CASE_E(X,				'X')
		CASE_E(Y,				'Y')
		CASE_E(Z,				'Z')
		CASE_E(UpArrow,			VK_UP)
		CASE_E(DownArrow,		VK_DOWN)
		CASE_E(LeftArrow,		VK_LEFT)
		CASE_E(RightArrow,		VK_RIGHT)
		CASE_E(Insert,			VK_INSERT)
		CASE_E(Delete,			VK_DELETE)
		CASE_E(Home,			VK_HOME)
		CASE_E(End,				VK_END)
		CASE_E(PageUp,			VK_PRIOR)
		CASE_E(PageDown,		VK_NEXT)
		CASE_E(Keypad0,			VK_NUMPAD0)
		CASE_E(Keypad1,			VK_NUMPAD1)
		CASE_E(Keypad2,			VK_NUMPAD2)
		CASE_E(Keypad3,			VK_NUMPAD3)
		CASE_E(Keypad4,			VK_NUMPAD4)
		CASE_E(Keypad5,			VK_NUMPAD5)
		CASE_E(Keypad6,			VK_NUMPAD6)
		CASE_E(Keypad7,			VK_NUMPAD7)
		CASE_E(Keypad8,			VK_NUMPAD8)
		CASE_E(Keypad9,			VK_NUMPAD9)
		CASE_E(PrintScreen,		VK_SNAPSHOT)
		CASE_E(ScrollLock,		VK_SCROLL)
		CASE_E(Pause,			VK_PAUSE)
		CASE_E(NumLock,			VK_NUMLOCK)
		CASE_E(KeypadDivide,	VK_DIVIDE)	 // /
		CASE_E(KeypadMultiply,	VK_MULTIPLY) // *
		CASE_E(KeypadMinus,		VK_SUBTRACT) // -
		CASE_E(KeypadPlus,		VK_ADD)		 // +
		CASE_E(KeypadPeriod,	VK_DECIMAL)	 // .
	}
#undef CASE_E
	return 0;
}

Win32_ErrorCodeString::Win32_ErrorCodeString(::DWORD errorCode) {
// retrieving-error-messages: https://docs.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages
// system error code lookup: https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes

	TempStringW tmp;
	tmp.resizeToLocalBufSize();
	auto dwChars = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
								 , NULL
								 , errorCode
								 , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
								 , tmp.data()
								 , static_cast<::DWORD>(tmp.size())
								 , NULL);

	if (dwChars) {
		UtfUtil::convert(_str, tmp);
	}
}

void Win32_ErrorCodeString::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}", _str);
}

} // namespace axe

#endif // AXE_OS_WINDOWS