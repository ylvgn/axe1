#pragma once

#pragma warning(disable: 4668) // 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'

#if !AXE_COMPILER_VC	
	#error
#endif

#if _MSVC_LANG >= 199711L
	#define AXE_CPLUSPLUS_03	1
#endif

#if _MSVC_LANG >= 201103L
	#define AXE_CPLUSPLUS_11	1
#endif

#if _MSVC_LANG >= 201402L
	#define AXE_CPLUSPLUS_14	1
#endif

#if _MSVC_LANG >= 201703L
	#define AXE_CPLUSPLUS_17	1
#endif

#if _MSVC_LANG >= 202002L
	#define AXE_CPLUSPLUS_20	1
#endif

#define AXE_TYPE_HAS_SIZE_T		0
#define AXE_TYPE_HAS_SSIZE_T	0
#define AXE_TYPE_HAS_LONGLONG	0

#define AXE_DLL_EXPORT			_declspec(dllexport)
#define AXE_DLL_IMPORT			_declspec(dllimport)

#define	AXE_FUNC_NAME_SZ			__FUNCTION__
#define AXE_FUNC_FULLNAME_SZ		__FUNCSIG__

#define AXE_DEPRECATED			__declspec(deprecated)

#define AXE_COMPILER_VER _MSVC_LANG

#if AXE_CPLUSPLUS_17
	#define AXE_FALLTHROUGH	//	[[fallthrough]]
	#define AXE_NODISCARD		[[nodiscard]]
#else
	#define AXE_FALLTHROUGH
	#define AXE_NODISCARD
#endif

//#define AXE_ALIGN(N)				__declspec(align(N)) 
//#define AXE_ALIGN(N)				alignas(N) //c++11

#define AXE_OPTIMIZE_OFF			__pragma(optimize("", off))

#if 0 // AXE_GEN_CONFIG_Debug
	#define	AXE_INLINE		inline
#else
	#define	AXE_INLINE		__forceinline
#endif

#define AXE_THREAD_LOCAL			__declspec( thread )
//#define AXE_THREAD_LOCAL thread_local //C++11

#if _MSC_VER < 1600
	#define	nullptr	NULL
#endif

//cpu
#if _M_X64
	#define AXE_CPU_X86_64	1
	#define AXE_CPU_FEATURE_SSE1		1
	#define AXE_CPU_FEATURE_SSE2		1
	#define AXE_CPU_FEATURE_SSE3		1

#elif _M_PPC
	#define AXE_CPU_PowerPC     1
#else
	#define AXE_CPU_x86         1
#endif

//os

#if _WIN64
	#define AXE_OS_WIN64     1
	#define AXE_OS_WINDOWS	1
#elif _WIN32
	#define AXE_OS_WIN32     1
	#define AXE_OS_WINDOWS	1
#elif _WINCE
	#define AXE_OS_WinCE     1
	#define AXE_OS_WINDOWS	1
#endif
