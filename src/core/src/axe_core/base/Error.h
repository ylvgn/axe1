#pragma once

#include "../string/Fmt.h"

#define AXE_ERROR(...) Error(AXE_LOC, Fmt(__VA_ARGS__))

namespace axe {

using IError = ::std::exception;
class Error : public IError {
	using Base = IError;
	using This = Error;

	Error()	   = delete;

public:
	Error(const SrcLoc& loc, StrView msg);

	void onFormat(fmt::format_context& ctx) const;

	static void s_setEnableAssertion(bool b);

private:
	SrcLoc	   _loc;
	TempString _msg;

	static bool s_enableAssertion;
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