#pragma once

#include "../string/Fmt.h"

#define AXE_ERROR(...) Error(AXE_LOC, Fmt(__VA_ARGS__))

namespace axe {

class IError : public std::exception {
public:

};

class Error : public IError {
	Error() = delete;
public:
	Error(const SrcLoc& loc, StrView msg);

	void onFormat(fmt::format_context& ctx) const;

private:
	SrcLoc		_loc;
	TempString	_msg;
};
AXE_FORMATTER(Error)

class Error_Undefined : public Error {
	using Base = Error;
public:
	explicit Error_Undefined(const SrcLoc& loc, StrView msg = StrView()) : Error(loc, msg) {}
};

} // namespace axe