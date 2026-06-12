#include "Quat4.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#else
	template Quat4<float>;
	template Quat4<double>;
#endif

} // namespace axe