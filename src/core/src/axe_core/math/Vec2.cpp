#include "Vec2.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template Vec2_SSE<float>;
	template Vec2_SSE<double>;

#else
	template Vec2_Basic<float>;
	template Vec2_Basic<double>;
#endif

}