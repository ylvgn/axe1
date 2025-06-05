#include "Error.h"
#include <axe_core/log/Log.h>

namespace axe {

Error::Error(const SrcLoc& loc, StrView msg)
	: _loc(loc), _msg(msg)
{
	AXE_LOG("{}", *this);
	AXE_ASSERT(false);
}

void Error::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "Error: {}\n{}", _loc, _msg);
}

} // namespace axe
