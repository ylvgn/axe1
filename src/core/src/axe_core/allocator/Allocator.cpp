#include "Allocator.h"
#include <axe_core/base/axe_base.h>

#if TRACY_ENABLE
	#include <common/TracySystem.hpp>
#endif

namespace axe {

AllocatorChunkBase::AllocatorChunkBase(Allocator* allocator)
	: _allocator(allocator)
{
}

} // namespace axe