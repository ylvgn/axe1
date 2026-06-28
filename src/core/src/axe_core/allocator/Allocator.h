#pragma once

//----- Global namespace
#if !EASTL_DLL
// If building a regular library and not building EASTL as a DLL...
// It is expected that the application define the following
// versions of operator new for the application. Either that or the
// user needs to override the implementation of the allocator class.
AXE_INLINE void* operator new[](size_t		numBytes
							  , const char*	pName
							  , int			flags
							  , unsigned	debugFlags
							  , const char*	file
							  , int			line)
{
	void* p = ::malloc(numBytes);
	AXE_TracyAlloc(p, numBytes);
	return p;
}

AXE_INLINE void* operator new[](size_t		numBytes
							  , size_t		alignment
							  , size_t		alignmentOffset
							  , const char*	pName
							  , int			flags
							  , unsigned	debugFlags
							  , const char*	file
							  , int			line)
{
	void* p = nullptr;
#if AXE_OS_WINDOWS
	p = ::_aligned_malloc(numBytes, alignment);
#else
	p = ::std::aligned_alloc(alignment, numBytes);
#endif
	AXE_TracyAlloc(p, numBytes);
	return p;
}

void* operator new(size_t numBytes);
void operator delete(void* p) noexcept;
void* operator new[](size_t numBytes);
void operator delete[](void* p) noexcept;
#endif // !EASTL_DLL


namespace axe {

class Allocator;
using IAllocator = ::eastl::allocator;

class AllocatorChunkBase : public NonCopyable {
protected:
	AllocatorChunkBase(Allocator* allocator);

	Allocator* _allocator;
}; // AllocatorChunkBase

class Allocator : public IAllocator {
	using Base = IAllocator;
	using This = Allocator;
protected:
	explicit Allocator(const char* name = "Unnamed")					: Base(name) {}
			 Allocator(const ::eastl::allocator& x)						: Base(x) {}
			 Allocator(const ::eastl::allocator& x, const char* pName)	: Base(x, pName) {}
}; // Allocator

} // namespace axe