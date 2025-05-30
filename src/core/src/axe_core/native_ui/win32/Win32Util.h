#pragma once

#include <axe_core/string/StringUtil.h>
#include <axe_core/math/Rect2.h>

#if AXE_OS_WINDOWS

namespace axe {

enum class UIKeyboardEventKeyCode : u32;

class Win32Util {
	Win32Util() = delete;
	using KeyCode = UIKeyboardEventKeyCode;
public:
	static void errorTo(String& out, ::DWORD in_errorcode = ::WSAGetLastError());
	static void errorTo(String& out, ::HRESULT hr) { errorTo(out, static_cast<::DWORD>(hr)); }

	static String error(::DWORD in_errorcode = ::WSAGetLastError()) { String o; errorTo(o, in_errorcode); return o; }
	static String error(::HRESULT hr) { return error(static_cast<::DWORD>(hr)); }

	static void convert(Rect2f& o, const ::RECT& i) {
		o.x = static_cast<float>(i.left);
		o.y = static_cast<float>(i.top);
		o.w = static_cast<float>(i.right - i.left);
		o.h = static_cast<float>(i.bottom - i.top);
	}

	static void convert(Rect2i& o, const ::RECT& i) {
		o.x = i.left;
		o.y = i.top;
		o.w = i.right - i.left;
		o.h = i.bottom - i.top;
	}

	static void convert(Vec2f& o, const ::POINT& i) {
		o.x = static_cast<float>(i.x);
		o.y = static_cast<float>(i.y);
	}

	static void convert(Vec2i& o, const ::POINT& i) {
		o.x = i.x;
		o.y = i.y;
	}

	static void convert(Vec2f& o, const ::SIZE& i) {
		o.x = static_cast<float>(i.cx);
		o.y = static_cast<float>(i.cy);
	}

	static void convert(Vec2i& o, const ::SIZE& i) {
		o.x = static_cast<int>(i.cx);
		o.y = static_cast<int>(i.cy);
	}

	static void convert(::RECT& o, const Rect2f& i) {
		o.left   = static_cast<LONG>(i.x);
		o.top    = static_cast<LONG>(i.y);
		o.right  = static_cast<LONG>(i.xMax());
		o.bottom = static_cast<LONG>(i.yMax());
	}

	static void convert(::POINT& o, const Vec2f& i) {
		o.x = static_cast<LONG>(i.x);
		o.y = static_cast<LONG>(i.y);
	}

	static void convert(::POINT& o, const Vec2i& i) {
		o.x = static_cast<LONG>(i.x);
		o.y = static_cast<LONG>(i.y);
	}

	static void convert(::SIZE& o, const Vec2f& i) {
		o.cx = static_cast<LONG>(i.x);
		o.cy = static_cast<LONG>(i.y);
	}

	static void convert(::SIZE& o, const Vec2i& i) {
		o.cx = static_cast<LONG>(i.x);
		o.cy = static_cast<LONG>(i.y);
	}

	static Rect2f toRect2f(const ::RECT& i)		{ Rect2f o; convert(o, i); return o; }
	static Rect2i toRect2i(const ::RECT& i)		{ Rect2i o; convert(o, i); return o; }
	static	Vec2f  toVec2f(const ::POINT& i)	{ Vec2f  o; convert(o, i); return o; }
	static	Vec2i  toVec2i(const ::POINT& i)	{ Vec2i  o; convert(o, i); return o; }
	static	Vec2f  toVec2f(const ::SIZE& i)		{ Vec2f  o; convert(o, i); return o; }
	static	Vec2i  toVec2i(const ::SIZE& i)		{ Vec2i  o; convert(o, i); return o; }

	static ::POINT	toPoint(const Vec2f& i)		{ ::POINT o; convert(o, i); return o; }
	static ::RECT	toRect(const Rect2f& i)		{ ::RECT o;  convert(o, i); return o; }

	static int		toVKKey(const KeyCode& i);
}; // Win32Util
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(Win32Util);

} // namespace axe


template<>
struct fmt::formatter<::RECT> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const ::RECT& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "(left={}, top={}, right={}, bottom={})", v.left, v.top, v.right, v.bottom);
	}
};

template<>
struct fmt::formatter<::POINT> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const ::POINT& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "({}, {})", v.x, v.y);
	}
};

template<>
struct fmt::formatter<::SIZE> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const ::SIZE& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "({}, {})", v.cx, v.cy);
	}
};

#endif // AXE_OS_WINDOWS