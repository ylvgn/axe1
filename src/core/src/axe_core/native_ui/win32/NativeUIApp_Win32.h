#pragma once
#if AXE_OS_WINDOWS

#include "../base/NativeUIApp_Base.h"

namespace axe {

class NativeUIApp_Win32 : public NativeUIApp_Base {
	AXE_RTTI_INFO(NativeUIApp_Win32, NativeUIApp_Base)
public:
	AXE_DOWNCAST_GET_INSTANCE()

	NativeUIApp_Win32(const CreateDesc& desc) : Base(desc) {}

	virtual void quit	(int returnCode)	override;
	virtual int  onRun	()					override;

	::MSG _win32_msg;

private:
	DWORD _getMonitorDisplayFrequency();
}; // NativeUIApp_Win32

} // namespace axe

#endif // AXE_OS_WINDOWS