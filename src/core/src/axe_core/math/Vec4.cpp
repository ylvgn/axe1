#include "Vec4.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template Vec4_SSE<float>;
	template Vec4_SSE<double>;
#else
	template struct Vec4_Basic<float>;
	template struct Vec4_Basic<double>;
#endif

} // namespace axe