#include "MemMapFile.h"

namespace axe {

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

void MemMapFile::open(StrView filename) {
	close();
	_fs.openRead(filename);

	if (_fs.fileSize() >= SIZE_T_MAX)
		throw AXE_ERROR("memmap file size too larget");

	auto size = static_cast<size_t>(_fs.fileSize());
	if (size <= 0) {
		close();
		return;
	}

	_mapping = ::CreateFileMapping(_fs.nativeFd(), nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!_mapping) {
		throw AXE_ERROR("::CreateFileMapping error");
	}

	auto* data = reinterpret_cast<u8*>(::MapViewOfFile(_mapping, FILE_MAP_READ, 0, 0, 0));
	if (!data) {
		throw AXE_ERROR("::MapViewOfFile error");
	}

	_span = ByteSpan(data, size);
}

void MemMapFile::close() {
	if (_span.size() <= 0) return;
	::UnmapViewOfFile(_span.data());

	if (_mapping) {
		::CloseHandle(_mapping);
		_mapping = nullptr;
	}

	_span = ByteSpan();
	_fs.close();
}

#else

void MemMapFile::open(StrView filename) {
	close();
	_fs.openRead(filename);

	if (_fs.fileSize() >= SIZE_T_MAX)
		throw AXE_ERROR("memmap file size too larget");

	auto size = static_cast<size_t>(_fs.fileSize());
	if (size <= 0) return;

	auto* data = reinterpret_cast<u8*>(::mmap(0, size, PROT_READ, MAP_PRIVATE, _fs.nativeFd(), 0));
	if (data == MAP_FAILED || data == nullptr) {
		throw AXE_ERROR("memmap");
	}
	_span = ByteSpan(data, size);
}

void MemMapFile::close() {
	if (_span.data()) {	
		::munmap(_span.data(), _span.size());
	}
	_span = ByteSpan();
	_fs.close();
}

#endif

} // namespace axe