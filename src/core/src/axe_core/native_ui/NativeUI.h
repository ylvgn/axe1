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
public:
	NativeUIApp() noexcept;
	~NativeUIApp() noexcept;

	static NativeUIApp* current();
private:
	static NativeUIApp* _current;
}; // NativeUIApp

} // namespace axe


//----
template<class T, class ENABLE = void>
struct axeMain {
	axeMain() = delete;

	static int run(T& app) {
		AXE_STATIC_ASSERT( ::axe::is_base_of_v< ::axe::ConsoleApp, T> );
		app.onRun();
		return app.exitCode();
	}
};

template<class T>
struct axeMain<T, ::axe::enable_if_t< ::axe::is_base_of_v< ::axe::NativeUIApp, T>> > {
	axeMain() = delete;

	static int run(T& app) {
		T::CreateDesc desc;
		app.run(desc);
		return app.exitCode();
	}
};

#define AXE_MAIN(T) \
	int main(int argc, const char* argv[]) { \
		try { \
			T app; \
			app.setCommandArguments(argc, argv); \
			return axeMain<T>::run(app); \
		} \
		catch (...) { \
			AXE_ASSERT(false); \
			throw; \
		} \
	} \
//----
