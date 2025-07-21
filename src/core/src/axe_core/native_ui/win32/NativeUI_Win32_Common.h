#pragma once

#if AXE_OS_WINDOWS

#include "../NativeUI_Common.h"
#include <axe_core/string/StringUtil.h>
#include <axe_core/math/Rect2.h>

#define AXE_WIN32_THROW_SYSTEM_ERROR(dwSystemErrorCode) throw Error_Win32_ErrorCodeString(AXE_LOC, dwSystemErrorCode)
#define AXE_WIN32_THROWIF_LAST_ERROR()                         \
	do                                                         \
	{                                                          \
		auto _axe_tmp_error_code = ::WSAGetLastError();        \
		if (_axe_tmp_error_code)                               \
		{                                                      \
			AXE_WIN32_THROW_SYSTEM_ERROR(_axe_tmp_error_code); \
		}                                                      \
	} while (false) \
//----

#define AXE_WIN32_THROWIF_HRESULT_ERROR(hr)                         \
	do                                                              \
	{                                                               \
		auto _axe_tmp_hr = (hr);                                    \
		if (FAILED(_axe_tmp_hr))                                    \
		{                                                           \
			throw Error_Win32_HRESULT_String(AXE_LOC, _axe_tmp_hr); \
		}                                                           \
	} while (false) \
//----

namespace axe {

class Win32Util : public NativeUICommonBase {
public:
	static void convert(  Rect2f& o, const ::RECT   & i);
	static void convert(::RECT	& o, const   Rect2f & i);

	static void convert(  Rect2i& o, const ::RECT   & i);
	static void convert(::RECT  & o, const   Rect2i & i);

	static void convert(  Vec2f	& o, const ::POINT  & i);
	static void convert(::POINT	& o, const   Vec2f  & i);

	static void convert(::POINT	& o, const   Vec2i  & i);
	static void convert(  Vec2i	& o, const ::POINT  & i);

	static void convert(  Vec2f	& o, const ::SIZE   & i);
	static void convert(::SIZE	& o, const   Vec2f  & i);

	static void convert(  Vec2i	& o, const ::SIZE   & i);
	static void convert(::SIZE	& o, const   Vec2i  & i);

	static void convert(u64& o, const ::LARGE_INTEGER& i);

	static Rect2f toRect2f(const ::RECT&  i)	{ Rect2f o; convert(o, i); return o; }
	static Rect2i toRect2i(const ::RECT&  i)	{ Rect2i o; convert(o, i); return o; }
	static	Vec2f  toVec2f(const ::POINT& i)	{ Vec2f  o; convert(o, i); return o; }
	static	Vec2i  toVec2i(const ::POINT& i)	{ Vec2i  o; convert(o, i); return o; }
	static	Vec2f  toVec2f(const ::SIZE&  i)	{ Vec2f  o; convert(o, i); return o; }
	static	Vec2i  toVec2i(const ::SIZE&  i)	{ Vec2i  o; convert(o, i); return o; }

	static ::POINT	toPoint(const Vec2f& i)		{ ::POINT o; convert(o, i); return o; }
	static ::RECT	toRect(const Rect2f& i)		{ ::RECT  o; convert(o, i); return o; }

	static int		toVKKey(const KeyCode& i);
}; // Win32Util
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(Win32Util);


#if 0
#pragma mark ========= Win32_Handle ============
#endif
class Win32_Handle : public NonCopyable {
public:
	Win32_Handle() = default;
	explicit Win32_Handle(::HANDLE h) noexcept : _h(h) {}

	~Win32_Handle();

	void set(::HANDLE h);
	void unref();

	AXE_NODISCARD ::HANDLE get() { return _h; }
	operator	  ::HANDLE()	 { return _h; }

	bool isValid() const { return _h != INVALID_HANDLE_VALUE; }

private:
	::HANDLE _h = INVALID_HANDLE_VALUE;
}; // Win32_Handle


#if 0
#pragma mark ========= Win32_ErrorCodeString ============
#endif
class Win32_ErrorCodeString {
	using This = Win32_ErrorCodeString;
public:
	Win32_ErrorCodeString(::DWORD errorCode);

	StrView		strView() const { return _str.view(); }
	operator	StrView() const	{ return strView(); }

	void onFormat(fmt::format_context& ctx) const;

private:
	String_<256> _str;
}; // Win32_ErrorCodeString
AXE_FORMATTER(Win32_ErrorCodeString)


#if 0
#pragma mark ========= Error_Win32_ErrorCodeString ============
#endif
class Error_Win32_ErrorCodeString : public Error {
	using This = Error_Win32_ErrorCodeString;
	using Base = Error;
public:
	explicit Error_Win32_ErrorCodeString(const SrcLoc& loc, ::DWORD errorCode) {
		_loc = loc;
		Win32_ErrorCodeString s(errorCode);
		FmtTo(_msg, "Win32 Error({}): {}\n{}", errorCode, s);
		Base::_assert();
	}
}; // Error_Win32_ErrorCodeString
AXE_FORMATTER(Error_Win32_ErrorCodeString)


#if 0
#pragma mark ========= Win32_HRESULT_String ============
#endif
class Win32_HRESULT_String {
	using This = Win32_ErrorCodeString;
public:
	Win32_HRESULT_String(::HRESULT hr);

	StrView		strView() const	{ return _str; }
	operator	StrView() const	{ return strView(); }

	void onFormat(fmt::format_context& ctx) const;

private:
	String_<256> _str;
}; // Win32_HRESULT_String
AXE_FORMATTER(Win32_HRESULT_String)


#if 0
#pragma mark ========= Error_Win32_HRESULT_String ============
#endif
class Error_Win32_HRESULT_String : public Error {
	using This = Error_Win32_HRESULT_String;
	using Base = Error;
public:
	explicit Error_Win32_HRESULT_String(const SrcLoc& loc, ::HRESULT hr)
	{
		_loc = loc;
		Win32_HRESULT_String s(hr);
		FmtTo(_msg, "HRESULT Error(0x{:0X}): {}", hr, s);
		Base::_assert();
	}
}; // Error_Win32_HRESULT_String
AXE_FORMATTER(Error_Win32_HRESULT_String)

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