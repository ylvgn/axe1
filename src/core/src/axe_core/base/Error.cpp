#include "Error.h"
#include <axe_core/log/Log.h>

namespace axe {

bool Error::s_enableAssertion = true;

Error::Error(const SrcLoc& loc, StrView msg)
	: _loc(loc)
	, _msg(msg)
{
	AXE_LOG("{}", *this);

	if (s_enableAssertion) {
		AXE_ASSERT(false);
	}
}

void Error::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Error: {}\n{}", _loc, _msg);
}

void Error::s_setEnableAssertion(bool b) {
	s_enableAssertion = b;
}

} // namespace axe
