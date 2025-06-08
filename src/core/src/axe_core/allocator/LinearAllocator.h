#pragma once

#include "Allocator.h"

namespace axe {

class LinearAllocator : public Allocator {
	using Base = Allocator;
	using This = LinearAllocator;
private:
	class Chunk : public AllocatorChunkBase {
		using Base = AllocatorChunkBase;
	public:
		Chunk(Allocator* allocator, size_t numBytes);

		void* allocate(size_t reqSize, size_t align);
		void  clear();

		LinearAllocator* allocator() const { return static_cast<LinearAllocator*>(_allocator); }
	private:
		Vector<u8>	_buffer;
		size_t		_used = 0;
	}; // Chunk

public:
	LinearAllocator();

	void	setChunkSize(size_t n) { _chunkSize = n; }

	void*	allocate(size_t reqSize, size_t align = 16);
	void	clear();

	Vector< UPtr<Chunk> > _chunks;
	size_t _chunkSize	= 16 * 1024;
}; // LinearAllocator

} // namespace axe