#pragma once

#if AXE_TRY_USE_DX12
	#if AXE_OS_WINDOWS
		#define AXE_RENDER_HAS_DX12 1
	#endif
#endif

#if AXE_RENDER_HAS_DX12
	#define AXE_RENDER_HAS_DX12_DXR 0
#endif