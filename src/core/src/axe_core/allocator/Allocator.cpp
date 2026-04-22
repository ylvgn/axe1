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

//----- Global namespace
void* operator new(size_t numBytes) {
	auto* p = ::malloc(numBytes);
	AXE_TracyAlloc(p, numBytes);
	return p;
}

void operator delete(void* p) noexcept {
	AXE_TracyFree(p);
	::free(p);
}

void* operator new[](size_t numBytes) {
	auto* p = ::malloc(numBytes);
	AXE_TracyAlloc(p, numBytes);
	return p;
}

void operator delete[](void* p) noexcept {
	AXE_TracyFree(p);
	::free(p);
}