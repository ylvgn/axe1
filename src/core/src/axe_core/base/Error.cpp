#include "Error.h"
#include <axe_core/log/Log.h>

namespace axe {

bool Error::_s_enableAssertion = true;

Error::Error(const SrcLoc& loc, StrView msg)
	: _loc(loc)
	, _msg(msg)
{
	AXE_LOG("{}", *this);
	AXE_ASSERT(false);
}

void Error::onFormat(fmt::format_context& ctx) const {
	if (!_msg)
		fmt::format_to(ctx.out(), "[Error] {}", _loc);
	else
		fmt::format_to(ctx.out(), "[Error] {}\n  - {}", _msg, _loc);
}

void Error::s_setEnableAssertion(bool b) {
	_s_enableAssertion = b;
}

void Error::s_assert(const SrcLoc& srcLoc, StrView expr, StrView msg, StrView title) {
	Error::s_assert(srcLoc.func, srcLoc.file, srcLoc.line, expr, msg, title);
}

void Error::s_assert(StrView funcName
				   , StrView filename
				   , int lineNumber
				   , StrView expr
				   , StrView msg
				   , StrView title)
{
	::std::cout << "\n[" << title << "]\n"
				<< "  EXPR: " << expr << "\n"
				<< "  Source: " << filename << ":" << lineNumber << " - " << funcName << "\n"
				<< msg << "\n";
	AXE_DEBUG_BREAK();
	if (_s_enableAssertion) {
		assert(false);
	}
}

} // namespace axe
