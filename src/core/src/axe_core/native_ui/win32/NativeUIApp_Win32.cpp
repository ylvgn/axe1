#if AXE_OS_WINDOWS

#include "NativeUIApp_Win32.h"
#include "../NativeUI.h"
#include "../../string/UtfUtil.h"

namespace axe {

int NativeUIApp_Win32::onRun() {
	
	create();
	

	for (;;) {
		if (_desc.peekMessage) {
			if (!::PeekMessage(&_win32_msg, nullptr, 0, 0, PM_REMOVE)) {
				onPeekMessage();
				continue;
			}
		} else {
			if (!::GetMessage(&_win32_msg, nullptr, 0, 0)) break;
		}

		if (_win32_msg.message == WM_QUIT) break;

		::TranslateMessage(&_win32_msg);
		::DispatchMessage(&_win32_msg);
	}

#if 0 // TODO should move to EditorApp about fps, update freame feature
	setFps(_getMonitorDisplayFrequency());
	_tickCount			= ::GetTickCount64();
	_win32_msg.message	= static_cast<UINT>(~WM_QUIT);
	
	while (_win32_msg.message != WM_QUIT) {
		if (PeekMessage(&_win32_msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&_win32_msg);
			DispatchMessage(&_win32_msg);
		} else {
			auto thisTickCount = ::GetTickCount64();
			float deltaTime = static_cast<float>(thisTickCount - _tickCount) * 0.001f;
			_tickCount = thisTickCount;
			update(deltaTime);
		}
	}
#endif
	
	willQuit();
	return _returnCode;
}

void NativeUIApp_Win32::quit(int returnCode) {
	_returnCode = returnCode;
	::PostQuitMessage(_returnCode);
}

DWORD NativeUIApp_Win32::_getMonitorDisplayFrequency() {
	::POINT ptZero = { 0, 0 };
	::HMONITOR hMonitor = ::MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

	::MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(monitorInfo);
	::GetMonitorInfo(hMonitor, &monitorInfo);

	::DEVMODE devMode;
	devMode.dmSize = sizeof(devMode);
	devMode.dmDriverExtra = 0;
	::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);
	return devMode.dmDisplayFrequency;
}

} // namespace axe

#endif // AXE_OS_WINDOWS
