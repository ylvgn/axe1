#pragma once

#include "../string/Fmt.h"

namespace axe {

using IError = ::std::exception;

class Error : public IError {
	using Base = IError;
	using This = Error;
public:
	static void s_setEnableAssertion(bool b);
	static bool s_getEnableAssertion() { return _s_enableAssertion; }

	static void s_setEnableDebugBreak(bool b);
	static bool s_getEnableDebugBreak() { return _s_enableDebugBreak; }

	static void s_assert(const SrcLoc& srcLoc, StrView expr, StrView msg, StrView title);
	static void s_assert(StrView funcName
					   , StrView filename
					   , int	 lineNumber
					   , StrView expr
					   , StrView msg = StrView()
					   , StrView title = "---- ERROR ASSERT ----");

	static bool s_validate(StrView	funcName
						 , StrView	filename
						 , int		lineNumber
						 , bool		validation
						 , StrView	expr
						 , StrView	msg = StrView());

	Error(const SrcLoc& loc);
	Error(StrView msg, const SrcLoc& loc);

	void onFormat(fmt::format_context& ctx) const;

protected:
	Error() = default; // no need create error, throw it when created

	void _assert();

	TempString _msg;
	SrcLoc	   _loc;
private:
	static bool _s_enableAssertion;
	static bool _s_enableDebugBreak;
}; // Error
AXE_FORMATTER(Error)

AXE_SIMPLE_ERROR(Error_Undefined)
AXE_SIMPLE_ERROR(Error_SafeCast)

} // namespace axe