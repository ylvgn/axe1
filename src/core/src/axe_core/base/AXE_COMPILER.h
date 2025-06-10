#pragma once

#if AXE_COMPILER_VC
	#include "AXE_COMPILER_VC.h"
#elif AXE_COMPILER_GCC
	#include "AXE_COMPILER_GCC.h"
#endif
//----

// usage:
//     AXE_DISABLE_VC_WARNING(4127 3244)
//     <code>
//     AXE_RESTORE_VC_WARNING()
//
#if AXE_COMPILER_VC
	#define AXE_DISABLE_VC_WARNING(X)  \
		__pragma(warning(push))        \
		__pragma(warning(disable:X))
#else
	#define AXE_DISABLE_VC_WARNING()
#endif

#if AXE_COMPILER_VC
	#define AXE_RESTORE_VC_WARNING() __pragma(warning(pop))
#else
	#define AXE_RESTORE_VC_WARNING()
#endif
//----


#if defined(AXE_COMPILER_VC)
	#if defined(AXE_EXPORTS)
		#define AXE_API __declspec(dllexport)
	#elif defined(AXE_IMPORTS)
		#define AXE_API __declspec(dllimport)
	#else
		#define AXE_API
	#endif
#else
	#define AXE_API __attribute__((visibility("default")))
#endif
//----
