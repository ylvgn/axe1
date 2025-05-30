#include "Error.h"
#include <axe_core/log/Log.h>

namespace axe {

Error::Error(const SrcLoc& loc, StrView msg)
	: _loc(loc), _msg(msg)
{
	AXE_LOG("Error: {}", msg);
	AXE_ASSERT(false);
}

} // namespace axe
