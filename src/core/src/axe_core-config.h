#pragma once

#include <axe_core/detect_platform/axe_detect_platform.h>

#define AXE_MATH_USE_SSE	1

#define AXE_TRY_USE_DX12	1
#define AXE_TRY_USE_OPENGL	0 // would be removed in these days

#if AXE_OS_WINDOWS
	#define TRACY_ENABLE 1
	#define AXE_TRACY_CALLSTACK_DEPTH 32
#endif

#if TRACY_ENABLE
	#define AXE_TracyAlloc(PTR, SIZE)		TracyCAllocS(PTR, SIZE, AXE_TRACY_CALLSTACK_DEPTH)
	#define AXE_TracyFree(PTR)				TracyCFreeS(PTR, AXE_TRACY_CALLSTACK_DEPTH)

	#define AXE_FrameMark					FrameMark
	#define AXE_FrameMarkNamed(...)			FrameMarkNamed(__VA_ARGS__)

	#define AXE_ZoneScoped					ZoneScoped
	#define AXE_ZoneScopedN(...)			ZoneScopedN(__VA_ARGS__)
	#define AXE_ZoneText(...)				ZoneText(__VA_ARGS__)

	#define AXE_TracyGpuContext				TracyGpuContext
	#define AXE_TracyGpuContextName(...)	TracyGpuContextName(__VA_ARGS__)
	#define AXE_TracyGpuNamedZone(...)		TracyGpuNamedZone(__VA_ARGS__)
	#define AXE_TracyGpuNamedZoneC(...)		TracyGpuNamedZoneC(__VA_ARGS__)
	#define AXE_TracyGpuZone(...)			TracyGpuZone(__VA_ARGS__)
	#define AXE_TracyGpuZoneC(...)			TracyGpuZoneC(__VA_ARGS__)
	#define AXE_TracyGpuZoneTransient(...)	TracyGpuZoneTransient(__VA_ARGS__)
	#define AXE_TracyGpuCollect				TracyGpuCollect
#else
	#define AXE_TracyAlloc(...)
	#define AXE_TracyFree(...)

	#define AXE_FrameMark
	#define AXE_FrameMarkNamed(...)

	#define AXE_ZoneScoped
	#define AXE_ZoneScopedN(...)
	#define AXE_ZoneText(...)

	#define AXE_TracyGpuContext
	#define AXE_TracyGpuContextName(...)
	#define AXE_TracyGpuNamedZone(...)
	#define AXE_TracyGpuNamedZoneC(...)
	#define AXE_TracyGpuZone(...)
	#define AXE_TracyGpuZoneC(...)
	#define AXE_TracyGpuZoneTransient(...)
	#define AXE_TracyGpuCollect
#endif // TRACY_ENABLE