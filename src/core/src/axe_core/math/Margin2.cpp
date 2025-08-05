#include "Margin2.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#else
	template Margin2<int>;
	template Margin2<float>;
#endif

} // namespace axe