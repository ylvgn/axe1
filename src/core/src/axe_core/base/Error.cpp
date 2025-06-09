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

} // namespace axe
