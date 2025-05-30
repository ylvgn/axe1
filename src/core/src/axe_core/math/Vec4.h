#pragma once

#include "Vec3.h"
#include "Vec4_Basic.h"
#include "Vec4_SSE.h"

namespace axe {

#ifndef AXE_MATH_USE_SSE
	#error
#elif AXE_MATH_USE_SSE
	template<class T> using Vec4 = Vec4_SSE<T>;
#else
	template<class T> using Vec4 = Vec4_Basic<T>;
#endif

using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
//using Vec4i = Vec4<i32>;

//template<> const TypeInfo* TypeOf<Vec4f>();
//template<> const TypeInfo* TypeOf<Vec4d>();

}
