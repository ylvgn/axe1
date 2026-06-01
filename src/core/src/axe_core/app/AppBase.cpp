#include "AppBase.h"

namespace axe {

#if 0
#pragma mark ========= AppArguments ============
#endif

static AppArguments* AppArgments_instance;
	
AppArguments* AppArguments::s_instance() {
	return AppArgments_instance;
}

AppArguments::AppArguments(int argc, const char* argv[]) {
	AXE_ASSERT(!AppArgments_instance);
	AppArgments_instance = this;
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

AppArguments::~AppArguments() {
	AXE_ASSERT(AppArgments_instance == this);
	AppArgments_instance = nullptr;
}

#if 0
#pragma mark ========= AppBase ============
#endif

static AppBase* AppBase_instance;

AppBase* AppBase::s_instance() {
	return AppBase_instance;
}

AppBase::AppBase() noexcept {
	AXE_ASSERT(AppBase_instance == nullptr);
	AppBase_instance = this;
}

AppBase::~AppBase() noexcept {
	AXE_ASSERT(AppBase_instance == this);
	AppBase_instance = nullptr;
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