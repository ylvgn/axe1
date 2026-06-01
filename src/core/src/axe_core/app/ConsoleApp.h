#pragma once

#include "AppBase.h"

namespace axe {

class ConsoleApp : public AppBase {
public:
	void setConsoleTitle(StrView title) {
#if AX_OS_WINDOWS
		TempStringW titleW = UtfUtil::toStringW(title);
		::SetConsoleTitle(titleW.c_str());
#endif
	}
protected:
	
}; // ConsoleApp

} // namespace axe