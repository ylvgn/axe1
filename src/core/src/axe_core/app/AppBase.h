#pragma once
#include <axe_core/base/axe_macro.h>
#include <axe_core/string/UtfUtil.h>
#include <axe_core/file/FilePath.h>
#include <axe_core/file/Directory.h>

namespace axe {

#if 0
#pragma mark ========= AppArguments ============
#endif
class AppArguments : public NonCopyable {
public:
	explicit AppArguments(int argc, const char* argv[]);
	~AppArguments();
	
	static AppArguments* s_instance();

	Span<const StrView> args() const { return _argsView.span(); }

private:
	Vector<StrView, 8>	_argsView;
	Vector<String,  8>	_argsStr;
}; // AppArguments


#if 0
#pragma mark ========= AppBase ============
#endif
class AppBase : public NonCopyable {
public:
	AppBase() noexcept;
	virtual ~AppBase() noexcept;
	
	static AppBase* AppBase::s_instance();
	
	void			create()	{ onCreate(); }
	virtual void	onCreate() {}
	
	virtual int		onRun() = 0;
	virtual void	willQuit() {}

	void	setAppName(StrView s) { _appName = s; }
	StrView	appName();

	template<class STR> void executableFilenameTo(STR& out);
	String	executableFilename() { String o; executableFilenameTo(o); return o; }

	template<class STR> void executableDirPathTo(STR& out);
	String	executableDirPath() { String o; executableDirPathTo(o); return o; }

	template<class STR> void executableDirPathRelativeTo(STR& out, StrView relativePath);
	void	setCurDirRelativeToExecutable(StrView relativePath);

private:
	String	_appName;
}; // AppBase


template<class T>
struct App_run {
	int operator()(int argc, const char* argv[]) {
		try {
			axe::AppArguments arg(argc, argv);
			T app;
			int ret = app.onRun();
			return ret;
		} catch (...) {
			AXE_LOG_FLUSH();
			throw;
		}
	}
};

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

template<class STR> inline
void AppBase::executableFilenameTo(STR& out) {
	StringW_<MAX_PATH> pathW;
	pathW.resizeToLocalBufSize();
	auto requiredSize = ::GetModuleFileName(nullptr, pathW.data(), MAX_PATH);
	if (!requiredSize)
		AXE_THROW_ERROR("::GetModuleFileName error: {}", ::WSAGetLastError());
	pathW.resize(requiredSize);
	UtfUtil::convert(out, pathW);
	out.replaceChars('\\', '/');
}

template<class STR> inline
void AppBase::executableDirPathTo(STR& out) {
	executableFilenameTo(out);
	auto dir = FilePath::dirname(out.view());
	out.resize(dir.size());
}

template<class STR> inline
void AppBase::executableDirPathRelativeTo(STR& out, StrView relativePath) {
	executableDirPathTo(out);
	FilePath::combineTo(out, relativePath);
}
#endif // AXE_OS_WINDOWS

} // namespace axe