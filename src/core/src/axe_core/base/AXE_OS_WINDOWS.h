#pragma once

#if AXE_OS_WINDOWS

#define NOMINMAX 1
#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
#include <ws2tcpip.h> // struct sockaddr_in6
#include <Windows.h>
#include <intsafe.h>
#include <xmmintrin.h>

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
	// #include <Gl/glaux.h>
	// #pragma comment(lib, "GLaux.lib")
#endif // AXE_TRY_USE_OPENGL

#endif // AXE_OS_WINDOWS