#pragma once

#include "../string/StringUtil.h"

namespace axe {

#define SystemInfo_OS_ENUM_LIST(E) \
	E(Unknown, )                   \
	E(Windows, )                   \
	E(Android, )                   \
	E(Linux, )                     \
	E(MacOSX, )                    \
	E(iOS, )                       \
	E(Cygwin, )                    \
//----
AXE_ENUM_CLASS(SystemInfo_OS, u8)

class SystemInfo : public StaticClass {
public:
	using OS = SystemInfo_OS;

	static	OS			getOS();
	static	StrLiteral	getOSName();
	static	int			cpuCount();
}; // SystemInfo

} // namespace axe

