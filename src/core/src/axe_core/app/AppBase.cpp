#include "AppBase.h"

namespace axe {
#if 0
#pragma mark ========= AppArguments ============
#endif
AppArguments::AppArguments(int argc, const char* argv[]) {
#if !_DEBUG && AXE_OS_WINDOWS // for immutable command line params
	int argCount = 0;
	auto* arr = CommandLineToArgvW(GetCommandLineW(), &argCount);
	_argsStr.resize(argCount);
	for (int i = 0; i < argCount; ++i) {
		_argsStr[i] = UtfUtil::toString(arr[i]);
	}
#else
	_argsStr.resize(argc);
	for (int i = 0; i < argc; ++i) {
		_argsStr[i] = argv[i];
	}
#endif

	_argsView.resize(_argsStr.size());
	for (int i = 0; i < _argsStr.size(); ++i) {
		_argsView[i] = _argsStr[i];
	}
}

#if 0
#pragma mark ========= AppBase ============
#endif
void AppBase::setCommandArguments(int argc, const char* argv[]) {
	_args = UPtr_make<AppArguments>(argc, argv);
}

Span<const StrView> AppBase::commandArguments() const {
	return _args ? _args->args() : Span<const StrView>();
}

StrView AppBase::appName() {
	if (_appName.empty()) {
		executableFilenameTo(_appName);
	}
	return _appName;
}

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

void AppBase::setCurDirRelativeToExecutable(StrView relativePath) {
	TempString newCurDir;
	executableDirPathRelativeTo(newCurDir, relativePath);
	Directory::setCurrent(newCurDir);
}

#endif // AXE_OS_WINDOWS

} // namespace axe