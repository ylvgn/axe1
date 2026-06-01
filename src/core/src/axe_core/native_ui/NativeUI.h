#pragma once

#include "NativeUI_Common.h"

#if AXE_OS_WINDOWS
	#include "win32/NativeUIApp_Win32.h"
	#include "win32/NativeUIWindow_Win32.h"

	namespace axe {
		using NativeUIWindow_Impl	= NativeUIWindow_Win32;
		using NativeUIApp_Impl		= NativeUIApp_Win32;
	}
#else
	#error "unsupported platform"
#endif


namespace axe {

class NativeUIWindow : public NativeUIWindow_Impl {
}; // NativeUIWindow

class NativeUIApp : public NativeUIApp_Impl {
	using Base = NativeUIApp_Impl;
public:
	NativeUIApp(const CreateDesc& desc = CreateDesc()) : Base(desc) {}
}; // NativeUIApp

} // namespace axe


#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

#define AXE_APP_MAIN(T) \
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPWSTR lpCmdLine, int nCmdShow) { auto m = ::axe::App_run<T>(); return m(0, nullptr); } \
	int main(int argc, const char* argv[]) { auto m = ::axe::App_run<T>(); return m(argc, argv); } \
//----

#else
#if 0
#pragma mark ========= Non-Windows ============
#endif

#define AXE_APP_MAIN(T) \
	int main(int argc, const char* argv[]) { auto m = ::axe::App_run<T>(); return m(argc, argv); } \
//----

#endif