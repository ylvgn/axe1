#pragma once

namespace axe {

class DirectoryEntry {
public:
	String name;
	String path;
	bool isDir  = false;
	bool hidden = false;
};

class Directory {
	Directory() = delete;
public:
	using Entry		 = DirectoryEntry;
	using FilterFunc = AXE_FUNC<bool(Entry& entry)>;

	static void		setCurrent(StrView dir);
	static void		currentTo(String& out);
	static String	current() { String o; currentTo(o); return o; }

	static void		create(StrView dir);
	static void		remove(StrView dir);
	static bool		exists(StrView dir);

	static void		getFileSystemEntries(Vector<Entry>& result, StrView path, bool subDir, FilterFunc filter);
	static void		getFiles(Vector<Entry>& result, StrView path, bool subDir, bool includeHiddenFile);
	static void		getDirectories(Vector<Entry>& result, StrView path, bool subDir, bool includeHiddenFile);

private:
	static void		_create(StrView dir);
	static void		_remove(StrView dir);
	static void		_appendGetFileSystemEntries(Vector<Entry>& result, StrView path, bool subDir, FilterFunc filter);
}; // Directory

AXE_STATIC_ASSERT_NO_MEMBER_CLASS(Directory);

} // namespace axe