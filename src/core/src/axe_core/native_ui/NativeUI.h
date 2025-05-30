#pragma once

#if AXE_OS_WINDOWS

#include "Win32/NativeUIApp_Win32.h"
#include "Win32/NativeUIWindow_Win32.h"

namespace axe {
	using NativeUIWindow_Impl	= NativeUIWindow_Win32;
	using NativeUIApp_Impl		= NativeUIApp_Win32;
}

#else
	#error "unsupported platform"

#endif


namespace axe {

class NativeUIWindow : public NativeUIWindow_Impl {
};

class NativeUIApp : public NativeUIApp_Impl {
public:
	NativeUIApp() noexcept;
	~NativeUIApp() noexcept;

	static NativeUIApp* current();
private:
	static NativeUIApp* _current;
}; // NativeUIApp

} // namespace axe


//------
template<class T, class ENABLE = void>
struct axeMain {
	axeMain() = delete;

	static int run(T& app) {
		AXE_STATIC_ASSERT(axe::TypeTraits::isBaseOf<axe::ConsoleApp, T>::value);
		app.onRun();
		return app.exitCode();
	}
};

template<class T>
struct axeMain<T, std::enable_if_t< axe::TypeTraits::isBaseOf<axe::NativeUIApp, T>::value> > {
	axeMain() = delete;

	static int run(T& app) {
		T::CreateDesc desc;
		app.run(desc);
		return app.exitCode();
	}
};

#if AXE_OS_WINDOWS
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
	//------
#else
	#define AXE_MAIN(T) \
		int main(int argc, const char* argv[]) { \
			T app; \
			app.setCommandArguments(argc, argv); \
			return axeMain<T>::run(app); \
		} \
	//------
#endif // AXE_OS_WINDOWS