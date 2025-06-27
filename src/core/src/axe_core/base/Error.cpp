#include "Error.h"
#include <axe_core/log/Log.h>

namespace axe {

bool Error::_s_enableAssertion  = true;
bool Error::_s_enableDebugBreak = true;

void Error::s_setEnableAssertion(bool b) {
	_s_enableAssertion = b;
}

void Error::s_setEnableDebugBreak(bool b) {
	_s_enableDebugBreak = b;
}

void Error::s_assert(const SrcLoc& srcLoc, StrView expr, StrView msg, StrView title) {
	Error::s_assert(srcLoc.func, srcLoc.file, srcLoc.line, expr, msg, title);
}

void Error::s_assert(StrView funcName
				   , StrView filename
				   , int	 lineNumber
				   , StrView expr
				   , StrView msg
				   , StrView title)
{
	::std::cout << "\n[" << title << "]\n"
				<< "  EXPR: " << expr << "\n"
				<< "  Source: " << filename << ":" << lineNumber << " - " << funcName << "\n"
				<< msg << "\n";

	if (_s_enableDebugBreak) {
		AXE_DEBUG_BREAK();
	}

	if (_s_enableAssertion) {
		assert(false);
	}
}

bool Error::s_validate(StrView funcName, StrView filename, int lineNumber, bool validation, StrView expr, StrView msg) {
	if (validation) return true;
	AXE_RUN_ONCE(Error::s_assert(funcName, filename, lineNumber, expr, msg, "---- VALIDATE ----"));
	return false;
}

Error::Error(const SrcLoc& loc, StrView msg)
	: _loc(loc)
	, _msg(msg)
{
	_assert();
}

void Error::onFormat(fmt::format_context& ctx) const {
	if (!_msg)
		fmt::format_to(ctx.out(), "[Error] {}", _loc);
	else
		fmt::format_to(ctx.out(), "[Error] {}\n  - {}", _msg, _loc);
}

void Error::_assert() {
	AXE_LOG("{}", *this);
	AXE_ASSERT(false);
}

} // namespace axe
