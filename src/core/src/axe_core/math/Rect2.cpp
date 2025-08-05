#include "Rect2.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#else
	template Rect2<int>;
	template Rect2<float>;
#endif

} // namespace axe