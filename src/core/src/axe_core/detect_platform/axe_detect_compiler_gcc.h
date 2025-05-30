#pragma once

#if ! (AXE_COMPILER_CLANG | AXE_COMPILER_GCC)
	#error
#endif

#if __cplusplus >= 199711L
	#define AXE_CPLUSPLUS_03	1
#endif

#if __cplusplus >= 201103L 
	#define AXE_CPLUSPLUS_11	1
#endif

#if __cplusplus >= 201402L
	#define AXE_CPLUSPLUS_14	1
#endif

#if __cplusplus >= 201703L
	#define AXE_CPLUSPLUS_17	1
#endif

#if __cplusplus >= 202002L
	#define AXE_CPLUSPLUS_20	1
#endif



#if AXE_COMPILER_CLANG
	#define AXE_TYPE_HAS_SIZE_T		0  //linux 
	#define AXE_TYPE_HAS_SSIZE_T		0  //linux 

	#define AXE_OPTIMIZE_OFF			_Pragma("clang optimize off")
#else
	#define AXE_TYPE_HAS_SIZE_T		0  //linux 
	#define AXE_TYPE_HAS_SSIZE_T		0  //linux 

	#define AXE_OPTIMIZE_OFF			_Pragma("GCC optimize(\"O0\")")
#endif

#if __GNUC__ >= 4
	#define	AXE_DLL_EXPORT			__attribute__ ((visibility ("default")))
	#define AXE_DLL_IMPORT			// __attribute__ ((visibility ("hidden")))
#else
	#define	AXE_DLL_EXPORT
	#define AXE_DLL_IMPORT
#endif


// #define nullptr	NULL

#define	AXE_FUNC_NAME_SZ				__FUNCTION__
#define AXE_FUNC_FULLNAME_SZ			__PRETTY_FUNCTION__

#define AXE_DEPRECATED( f )		f __attribute__( (deprecated) )

#define AXE_COMPILER_VER __cplusplus

#if AXE_CPLUSPLUS_17
	#define AXE_FALLTHROUGH		// [[fallthrough]]
	#define AXE_NODISCARD		[[nodiscard]]
#else
	#define AXE_FALLTHROUGH
	#define AXE_NODISCARD
#endif

//#define AXE_ALIGN(N)				__attribute__((aligned(N)))
//#define AXE_ALIGN(N)				alignas(N) //c++11

#if AXE_GEN_CONFIG_Debug
	#define	AXE_INLINE      inline
#else
	#define	AXE_INLINE		inline //__attribute__( (always_inline) )
#endif

//#define AXE_THREAD_LOCAL	__thread
#define AXE_THREAD_LOCAL thread_local //C++11


#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64) || defined(__amd64__)
	#define AXE_CPU_X86_64      1

#elif defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)
	#define AXE_CPU_X86         1

#elif defined(__POWERPC__)
	#define AXE_CPU_POWERPC     1

#elif defined(__aarch64__)
	#define AXE_CPU_ARM64 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define AXE_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define AXE_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif

#elif defined(__arm__)
	#define AXE_CPU_ARM 1

	#if defined(__ARMEL__) || defined(__AARCH64EL__)
		#define AXE_CPU_ENDIAN_LITTLE	1
	#elif defined(__ARMEB__) || defined(__AARCH64EB__)
		#define AXE_CPU_ENDIAN_BIG		1
	#else
		#error unknown ARM CPU endian
	#endif
#endif

#if	defined(__ARM_NEON__) || defined(__ARM_NEON)
	#define AXE_CPU_FEATURE_ARM_NEON
#endif

#if __SSE__
	#define AXE_CPU_FEATURE_SSE1			1
#endif

#if __SSE2__
	#define AXE_CPU_FEATURE_SSE2			1
	#include <xmmintrin.h>
#endif

#if __SSE3__
	#define AXE_CPU_FEATURE_SSE3			1
#endif

#if __SSSE3__
	#define AXE_CPU_FEATURE_SSSE3		1
#endif

#if __SSE4_1__
	#define AXE_CPU_FEATURE_SSE4_1		1
#endif

#if __SSE4_2__
	#define AXE_CPU_FEATURE_SSE4_2		1
#endif

//os

#if _WIN64
	#define AXE_OS_WIN64     1
	#define AXE_OS_WINDOWS	1

#elif _WIN32
	#define AXE_OS_WIN32     1
	#define AXE_OS_WINDOWS	1

#elif __ANDROID__
	#define AXE_OS_ANDROID	1

#elif __linux
	#define AXE_OS_LINUX        1

#elif __FreeBSD__
	#define AXE_OS_FREEBSD		1

#elif __APPLE__ && __MACH__
	#include <TargetConditionals.h>
	#if (TARGET_OF_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
		#define AXE_OS_IOS		1
	#else
		#define AXE_OS_MACOSX	1
	#endif

#elif __sun
	#define AXE_OS_SOLARIS		1

#elif __CYGWIN__
    #define AXE_OS_CYGWIN        1

#elif __MINGW32__
	#define AXE_OS_MINGW			1
#endif

