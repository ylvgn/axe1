#pragma once

#if AXE_OS_WINDOWS

#define NOGDICAPMASKS			// CC_*, LC_*, PC_*, CP_*, TC_*, RC_
//#define NOVIRTUALKEYCODES		// VK_*
//#define NOWINMESSAGES			// WM_*, EM_*, LB_*, CB_*
//#define NOWINSTYLES			// WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define NOSYSMETRICS			// SM_*
//#define NOMENUS				// MF_*
//#define NOICONS				// IDI_*
//#define NOKEYSTATES			// MK_*
//#define NOSYSCOMMANDS			// SC_*
//#define NORASTEROPS			// Binary and Tertiary raster ops
//#define NOSHOWWINDOW			// SW_*
#define OEMRESOURCE				// OEM Resource values
#define NOATOM					// Atom Manager routines
//#define NOCLIPBOARD			// Clipboard routines
//#define NOCOLOR				// Screen colors
//#define NOCTLMGR				// Control and Dialog routines
#define NODRAWTEXT				// DrawText() and DT_*
//#define NOGDI					// All GDI #defines and routines
#define NOKERNEL				// All KERNEL #defines and routines
//#define NOUSER				// All USER #defines and routines
//#define NONLS					// All NLS #defines and routines
//#define NOMB					// MB_* and MessageBox()
#define NOMEMMGR				// GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE				// typedef METAFILEPICT
#define NOMINMAX				// Macros min(a,b) and max(a,b)
//#define NOMSG					// typedef MSG and associated routines
#define NOOPENFILE				// OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL				// SB_* and scrolling routines
#define NOSERVICE				// All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND					// Sound driver routines
//#define NOTEXTMETRIC			// typedef TEXTMETRIC and associated routines
//#define NOWH					// SetWindowsHook and WH_*
//#define NOWINOFFSETS			// GWL_*, GCL_*, associated routines
#define NOCOMM					// COMM driver routines
#define NOKANJI					// Kanji support stuff.
#define NOHELP					// Help engine interface.
#define NOPROFILER				// Profiler interface.
#define NODEFERWINDOWPOS		// DeferWindowPos routines
#define NOMCX					// Modem Configuration Extensions
#define NOCRYPT
#define NOTAPE
#define NOIMAGE
#define NOPROXYSTUB
#define NORPC

#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#include <Windows.h>
#include <intsafe.h>
#include <xmmintrin.h>

struct IUnknown;	// All Com Obj inherit from IUnknown
#include <comdef.h> // Com error

// Win32 Version: https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
#if (_WIN32_WINNT >= _WIN32_WINNT_NT4)
	#define AXE_OS_WINDOWS_NT4_OR_LATER 1
#endif
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
	#define AXE_OS_WINDOWS_VISTA_OR_LATER 1
#endif
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)
	#define AXE_OS_WINDOWS_WIN7_OR_LATER 1
#endif
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8)
	#define AXE_OS_WINDOWS_WIN8_OR_LATER 1
#endif
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
	#define AXE_OS_WINDOWS_WIN10_OR_LATER 1
#endif

#ifndef AXE_TRY_USE_OPENGL
	#error
#elif AXE_TRY_USE_OPENGL
	#define GLEW_STATIC 1
	// include glew before gl.h
	#include <axe_core/glew/glew.h>
	#include <axe_core/glew/wglew.h>

	#include <GL/gl.h>
	#include <GL/glu.h>

// Note: glaux has been deprecated for a long time
//	#include <Gl/glaux.h>
//	#pragma comment(lib, "GLaux.lib")
#endif // AXE_TRY_USE_OPENGL

#endif // AXE_OS_WINDOWS