#pragma once

/*
AXE_DetectPlatform:
	- headers only
	- detect compiler / OS / CPU
	- may be used by external lib, so please keep it simple
*/

#ifdef __OBJC__ // Objective C
	#define AXE_OBJC		1
#else
	#define AXE_OBJC		0
#endif

//=========== Detect COMPILER ===============
#if defined(__clang__) 
	#define AXE_COMPILER_CLANG	1
	#include "axe_detect_compiler_gcc.h"

#elif defined(__GNUC__)
	#define AXE_COMPILER_GCC		1
	#include "axe_detect_compiler_gcc.h"

#elif defined(_MSC_VER)
	#define AXE_COMPILER_VC		1
	#include "axe_detect_compiler_vc.h"

#endif

#if AXE_COMPILER_VC + AXE_COMPILER_GCC + AXE_COMPILER_CLANG != 1
    #error "Compiler should be specified"
#endif

//======== Detect CPU =============

// check CPU define
#if AXE_CPU_X86_64 + AXE_CPU_X86 + AXE_CPU_POWERPC + AXE_CPU_ARM + AXE_CPU_ARM64 != 1
	#error "CPU should be specified"
#endif

#if AXE_CPU_X86_64
	#define AXE_CPU_LP64				1
	#define AXE_CPU_ENDIAN_LITTLE	1
	#define AXE_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AXE_CPU_X86
	#define AXE_CPU_LP32				1
	#define AXE_CPU_ENDIAN_LITTLE	1
	#define AXE_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AXE_CPU_POWERPC
	#define AXE_CPU_LP32				1
	#define AXE_CPU_ENDIAN_BIG		1
	#define AXE_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AXE_CPU_ARM64
	#define AXE_CPU_LP64				1
	#define AXE_CPU_MISALIGNED_MEMORY_ACCESSES	8
#endif

#if AXE_CPU_ARM
	#define AXE_CPU_LP32				1
	#define AXE_CPU_MISALIGNED_MEMORY_ACCESSES	1
#endif

#if AXE_CPU_LP32 + AXE_CPU_LP64 != 1
	#error "CPU bits should be specified"
#endif

#if AXE_CPU_LP32
	#define AXE_SIZEOF_POINTER		4
#endif

#if AXE_CPU_LP64
	#define AXE_SIZEOF_POINTER		8
#endif

#ifndef AXE_SIZEOF_POINTER
	#error "Unknown AXE_SIZEOF_POINTER"
#endif

#if AXE_CPU_ENDIAN_BIG + AXE_CPU_ENDIAN_LITTLE != 1
	#error "CPU endian should be specified"
#endif

//======== Detect OS ===============

#if AXE_OS_WIN32 + AXE_OS_WIN64 + AXE_OS_WINCE \
	+ AXE_OS_FREEBSD + AXE_OS_LINUX \
	+ AXE_OS_ANDROID \
	+ AXE_OS_MACOSX  + AXE_OS_IOS \
	+ AXE_OS_CYGWIN  + AXE_OS_MINGW != 1
	#error "OS should be specified"
#endif



//====================================

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif
