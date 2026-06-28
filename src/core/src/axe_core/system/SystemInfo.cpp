#include "SystemInfo.h"
#include <axe_core/base/axe_base.h>

namespace axe {

StrLit SystemInfo::getOSName() {
	return enumStr(getOS());
}

SystemInfo::OS SystemInfo::getOS() {
#if AXE_OS_WINDOWS
	return OS::Windows;
#elif AXE_OS_ANDROID
	return OS::Android;
#elif AXE_OS_MACOSX
	return OS::MacOSX;
#elif AXE_OS_IOS
	return OS::iOS;
#elif AXE_OS_LINUX
	return OS::Linux;
#elif AXE_OS_CYGWIN
	return OS::Cygwin;
#else
	#error "Unknown OS"
#endif
}

int SystemInfo::cpuCount() {
	return static_cast<int>(::std::thread::hardware_concurrency());
}

} // namespace axe