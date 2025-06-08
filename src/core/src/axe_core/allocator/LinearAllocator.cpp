#include "LinearAllocator.h"
#include "../math/Math.h"

namespace axe
{

LinearAllocator::LinearAllocator()
	: Base("LinearAllocator")
{
}

void* LinearAllocator::allocate(size_t reqSize, size_t align)
{
	if (_chunks.size()) {
		auto& t = _chunks.back();
		auto* p = t->allocate(reqSize, align);
		if (p) return p;
	}

	auto chunkSize = reqSize > _chunkSize ? reqSize : _chunkSize;

	auto newChunk = UPtr_make<Chunk>(this, chunkSize);
	_chunks.emplace_back(eastl::move(newChunk));

	auto* p = _chunks.back()->allocate(reqSize, align);
	return p;
}

void LinearAllocator::clear() {
	_chunks.clear();
}

LinearAllocator::Chunk::Chunk(Allocator* allocator, size_t numBytes)
	: Base(allocator)
{
	_buffer.resize(numBytes);
}

void* LinearAllocator::Chunk::allocate(size_t reqSize, size_t align) {
	auto offset = Math::alignTo(_used, align);
	auto available = _buffer.size() - offset;

	if (available < reqSize) {
		return nullptr;
	}

	_used = offset + reqSize;
	return _buffer.data() + offset;
}

void LinearAllocator::Chunk::clear() {
	_used = 0;
}

} // namespace axe