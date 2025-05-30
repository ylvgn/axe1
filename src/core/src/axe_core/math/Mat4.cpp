#include "Mat4.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template Mat4_SSE<float>;
	template Mat4_SSE<double>;

#else
	template Mat4_Basic<float>;
	template Mat4_Basic<double>;
#endif



}