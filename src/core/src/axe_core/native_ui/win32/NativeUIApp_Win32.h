#pragma once
#if AXE_OS_WINDOWS

#include "../base/NativeUIApp_Base.h"

namespace axe {

class NativeUIApp_Win32 : public NativeUIApp_Base {
	using Base = NativeUIApp_Base;
public:
	NativeUIApp_Win32(const CreateDesc& desc) : Base(desc) {}

	virtual void quit	(int returnCode)	override;
	virtual int  onRun	()					override;

	::MSG _win32_msg;

private:
	DWORD _getMonitorDisplayFrequency();
}; // NativeUIApp_Win32

} // namespace axe

#endif // AXE_OS_WINDOWS