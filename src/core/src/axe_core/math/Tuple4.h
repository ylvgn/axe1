#pragma once

#include "Tuple3.h"

namespace axe {

template<class T>
struct Tuple4 {
	axeTuple_InterfaceFunctions_Impl(Tuple4, T, 4)
	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	AXE_INLINE constexpr explicit Tuple4(T v)					 : x(v), y(v), z(v), w(v) {}
	AXE_INLINE constexpr explicit Tuple4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}

	AXE_INLINE constexpr void set(const Tuple4<T>& v) { *this = v; }
	AXE_INLINE constexpr void set(T x_, T y_, T z_, T w_) {
		x = x_; y = y_; z = z_; w = w_;
	}
	AXE_INLINE constexpr void setAll(const T& v)  { set(v,v,v,v); }
	AXE_INLINE constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}
};

using Tuple4f = Tuple4<float>;
using Tuple4d = Tuple4<double>;
using Tuple4i = Tuple4<i32>;

AXE_FORMATTER_T(class T, Tuple4<T>)

}