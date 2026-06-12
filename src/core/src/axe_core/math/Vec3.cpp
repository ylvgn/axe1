#include "Vec3.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template Vec3_SSE<float>;
	template Vec3_SSE<double>;
#else
	template struct Vec3_Basic<float>;
	template struct Vec3_Basic<double>;
#endif

} // namespace axe