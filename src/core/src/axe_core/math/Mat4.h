#pragma once

#include "Mat4_Basic.h"
#include "Mat4_SSE.h"

namespace axe {

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template<class T> using Mat4 = Mat4_SSE<T>;
#else
	template<class T> using Mat4 = Mat4_Basic<T>;
#endif

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

}
