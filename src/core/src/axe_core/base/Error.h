#pragma once

#include "../string/Fmt.h"

#define AXE_ERROR(...) Error(AXE_LOC, Fmt(__VA_ARGS__))
#define AXE_THROW(...) throw AXE_ERROR(__VA_ARGS__)

#define AXE_ASSERT_IMPL_SELECT(COUNT) AXE_ASSERT_IMPL_##COUNT
#define AXE_ASSERT_IMPL_1(expr)					  do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr); } } while(false)
#define AXE_ASSERT_IMPL_2(expr, msg)			  do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg); } } while(false)
#define AXE_ASSERT_IMPL_3(expr, msg, title)		  do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg, title); } } while(false)
#define AXE_ASSERT_IMPL_4(expr, msg, title, hack_expr) do{ if (!(expr)) { ::axe::Error::s_assert(__FUNCTION__, __FILE__, __LINE__, #expr, msg, title); { hack_expr; } } } while(false)
#define AXE_ASSERT_IMPL(...) AXE_IDENTITY(AXE_CALL(AXE_ASSERT_IMPL_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))

#define AXE_ASSERT_ONCE(...)   AXE_RUN_ONCE(AXE_ASSERT_IMPL(__VA_ARGS__))
#define AXE_ASSERT(expr)	   AXE_ASSERT_ONCE(expr, "", "---- ASSERT ----")
#define AXE_FATAL_ASSERT(expr) AXE_ASSERT_ONCE(expr, "", "---- FATAL ASSERT ----", axe_force_crash())

namespace axe {

using IError = ::std::exception;

class Error : public IError {
	using Base = IError;
	using This = Error;

	Error()	   = delete;

public:
	Error(const SrcLoc& loc, StrView msg = StrView());

	void onFormat(fmt::format_context& ctx) const;

	static void s_setEnableAssertion(bool b);
	static bool s_getEnableAssertion() { return _s_enableAssertion; }

	static void s_assert(StrView funcName, StrView filename, int lineNumber, StrView expr, StrView msg = StrView(), StrView title = "---- ASSERTION ----");
	static void s_assert(const SrcLoc& srcLoc, StrView expr, StrView msg, StrView title);

private:
	static bool _s_enableAssertion;

	SrcLoc	   _loc;
	TempString _msg;
}; // Error
AXE_FORMATTER(Error)


class Error_Undefined : public Error {
	using Base = Error;
	using This = Error_Undefined;
public:
	explicit Error_Undefined(const SrcLoc& loc, StrView msg = StrView()) : Base(loc, msg) {}
}; // Error_Undefined
AXE_FORMATTER(Error_Undefined)

} // namespace axe