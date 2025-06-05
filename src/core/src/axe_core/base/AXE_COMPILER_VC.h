#pragma once

#if AXE_COMPILER_VC

#define _CRT_SECURE_NO_WARNINGS 1

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Opengl32.lib") // gl.h
#pragma comment(lib, "Glu32.lib")	 // glu.h

#if _DEBUG
	#pragma comment(lib, "fmtd.lib")
#else
	#pragma comment(lib, "fmt.lib")
#endif

#endif // AXE_COMPILER_VC