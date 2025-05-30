#pragma once

#include "Vec2.h"
#include "Vec3_Basic.h"
#include "Vec3_SSE.h"

namespace axe {

#ifndef AXE_MATH_USE_SSE
	#error "Please include axe_core-config.h"
#elif AXE_MATH_USE_SSE
	template<class T> using Vec3 = Vec3_SSE<T>;
#else
	template<class T> using Vec3 = Vec3_Basic<T>;
#endif

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<i32>;

template<> const TypeInfo* TypeOf<Vec3f>();
//template<> const TypeInfo* TypeOf<Vec3d>();

}
