#include "Directory.h"
#include <axe_core/base/Error.h>
#include <axe_core/log/Log.h>
#include "FilePath.h"

#if AXE_OS_WINDOWS
	#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>
#endif

namespace axe {

void Directory::create(StrView dir) {
	if (exists(dir))
		return;

	auto parent = FilePath::dirname(dir);
	if (!parent.empty()) {
		create(parent); // create recursively
	}

	_create(dir);
}

void Directory::remove(StrView dir) {
	if (!exists(dir))
		return;

	_remove(dir);
}

void Directory::getFileSystemEntries(Vector<Entry>& result, StrView path, bool subDir, FilterFunc filter) {
	result.clear();
	_appendGetFileSystemEntries(result, path, subDir, filter);
}

void Directory::getFiles(Vector<Entry>& result, StrView path, bool subDir, bool includeHiddenFile) {
	result.clear();
	_appendGetFileSystemEntries(result, path, subDir, [&](Entry& entry){
		if (!includeHiddenFile && entry.hidden) return false;
		return !entry.isDir;
	});
}

void Directory::getDirectories(Vector<Entry>& result, StrView path, bool subDir, bool includeHiddenFile) {
	result.clear();
	_appendGetFileSystemEntries(result, path, subDir, [&](Entry& entry) {
		if (!includeHiddenFile && entry.hidden) return false;
		return entry.isDir;
	});
}

#if 0
#pragma mark ================= Windows ====================
#endif
#if AXE_OS_WINDOWS

class Directory_Win32Handle : public NonCopyable {
public:
	Directory_Win32Handle(::HANDLE h = INVALID_HANDLE_VALUE)
		: _h(h)
	{}

	~Directory_Win32Handle() { unref(); }

	bool isValid() const { return _h != INVALID_HANDLE_VALUE; }

	void set(::HANDLE h) { unref(); _h = h; }

	void unref() {
		if (isValid()) {
			::FindClose(_h);
			_h = INVALID_HANDLE_VALUE;
		}
	}

	operator ::HANDLE()	const { return _h; }

private:
	::HANDLE _h;
}; // Directory_Win32Handle

void Directory::setCurrent(StrView dir) {
	TempStringW pathW = UtfUtil::toStringW(dir);
	int ret = ::SetCurrentDirectory(pathW.c_str());
	if (!ret) {
		auto dwErrorCode = ::WSAGetLastError();
		switch (dwErrorCode) {
			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				AXE_LOG_WARN("The system cannot find the file specified: {}", dir); break;
			default: {
				AXE_LOG_ERROR("::SetCurrentDirectory invalid path: {}", dir);
				AXE_WIN32_THROW_SYSTEM_ERROR(dwErrorCode);
			} break;
		}
	}

#if defined(_DEBUG)
	AXE_DUMP_VAR(Directory::current());
#endif
}

void Directory::currentTo(String& out) {
	out.clear();

	StringW_<MAX_PATH> dirW;
	dirW.resizeToLocalBufSize();

	auto requiredSize = ::GetCurrentDirectory(MAX_PATH, dirW.data());
	if (!requiredSize) {
		AXE_WIN32_THROWIF_LAST_ERROR();
	}

	dirW.resize(requiredSize);
	UtfUtil::convert(out, dirW);
	out.replaceChars('\\', '/');
}

void Directory::_create(StrView dir) {
	TempStringW dirW;
	UtfUtil::convert(dirW, dir);
	auto ret = ::CreateDirectory(dirW.c_str(), nullptr);
	if (!ret) {
		auto dwErrorCode = ::WSAGetLastError();
		switch (dwErrorCode) {
			case ERROR_ALREADY_EXISTS:
				AXE_TODO("Just incase error from makefile, but unknow why happen")
				AXE_LOG_WARN("Cannot create a file when that file already exists: {}", dir); break;
			default: {
				AXE_LOG_ERROR("::CreateDirectory invalid path: {}", dir);
				AXE_WIN32_THROW_SYSTEM_ERROR(dwErrorCode);
			} break;
		}
	}
}

void Directory::_remove(StrView dir) {
	TempStringW dirW = UtfUtil::toStringW(dir);
	dirW += 0;

	// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/ns-shellapi-shfileopstructa
	::SHFILEOPSTRUCTW op = {};
	op.hwnd				 = NULL;
	op.wFunc			 = FO_DELETE;
	op.pFrom			 = dirW.c_str(); // This string must be double-null terminated.
	op.pTo				 = NULL;
	op.fFlags			 = FOF_ALLOWUNDO | FOF_NO_UI;

	::SHFileOperationW(&op);
}

bool Directory::exists(StrView dir) {
	TempStringW dirW;
	UtfUtil::convert(dirW, dir);
	::DWORD dwAttrib = ::GetFileAttributes(dirW.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void Directory::_appendGetFileSystemEntries(Vector<Entry>& result, StrView path, bool subDir, FilterFunc filter) {
	TempStringW pathW;
	auto pathA = TempString::s_format("{}/*", path.empty() ? "." : path);

	AXE_TODO("exceed the MAX_PATH limits TODO: https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file");
	if (pathA.size() >= MAX_PATH) {
		throw AXE_ERROR("path too long: path={}\nsize={}", pathA, pathA.size());
	}
	UtfUtil::convert(pathW, pathA);

	::WIN32_FIND_DATA data {0};
	Directory_Win32Handle h(::FindFirstFileEx(pathW.c_str(), FindExInfoBasic, &data, FindExSearchNameMatch, NULL, 0));

	if (!h.isValid())
		throw AXE_ERROR("::FindFirstFileEx INVALID_HANDLE_VALUE: path={}\n", path);

	do {
		auto filename = StrView_c_str(data.cFileName);

		if (filename == L"." || filename == L"..")
			continue;

		const auto& dwAttrib = data.dwFileAttributes;

		Entry e;
		UtfUtil::convert(e.name, filename);
		FilePath::combineTo(e.path, path, e.name);
		e.isDir  = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))	? true : false;
		e.hidden = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_HIDDEN))		? true : false;

		if (!filter || filter(e)) {
			result.emplace_back(AXE_MOVE(e));
		}

		if (e.isDir && subDir) {
			_appendGetFileSystemEntries(result, e.name, subDir, filter);
		}
	} while (::FindNextFile(h, &data));

	auto errorCode = ::WSAGetLastError();
	if (errorCode != ERROR_NO_MORE_FILES) {
		throw AXE_ERROR("unknown error: path={}\n", path);
	}
}

#if 0
#pragma mark =================  Non-Windows ====================
#endif
#else

Directory::_create(StrView dir) {
	TempStringA dirA;
	UtfUtil::convert(dirA, dir);
	auto ret = ::mkdir(dirA.c_str(), 0755);
	if (ret != 0)
		throw AXE_ERROR("::mkdir({}) error", dirA.c_str());
}

bool Directory::exists(StrView dir) {
	TempStringA dirA;
	UtfUtil::convert(dirA, dir);

	struct stat s;
	if( 0 != ::stat( dirA.c_str(), &s ) ) return false;
	return ( s.st_mode & S_IFDIR ) != 0;
}

#endif

} // namespace axe