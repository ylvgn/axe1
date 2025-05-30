#pragma once

#include "../base/NativeUIApp_Base.h"

#if AXE_OS_WINDOWS

namespace axe {

class NativeUIApp_Win32 : public NativeUIApp_Base {
	using Base = NativeUIApp_Base;
public:

	MSG _win32_msg;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onRun	() override;
	virtual	void onQuit	() override;

private:
	DWORD _getMonitorDisplayFrequency();
}; // NativeUIApp_Win32

} // namespace axe

#endif // AXE_OS_WINDOWS