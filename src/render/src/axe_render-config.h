#pragma once

#if AXE_TRY_USE_DX12
	#if AXE_OS_WINDOWS
		#define AXE_RENDER_HAS_DX12 1
	#endif
#endif

#if AXE_TRY_USE_VK
	#define AXE_RENDER_HAS_VK 0
#endif

#if AXE_RENDER_HAS_DX12
	#define AXE_RENDER_HAS_DX12_DXR 0
#endif

#if AXE_BUILD_CONFIG_Debug || AXE_BUILD_CONFIG_RelWithDebInfo
	#define AXE_RENDER_DEBUG_LAYER	1
#endif
