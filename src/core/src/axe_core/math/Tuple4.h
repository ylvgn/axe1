#pragma once

#include "Tuple3.h"

namespace axe {

template<class T>
class Tuple4 {
public:

	axeTuple_InterfaceFunctions_Impl(Tuple4, T, 4)
	union {
		struct { T x, y, z, w; };
		T data[kElementCount];
	};

	constexpr explicit Tuple4(T v)					  noexcept: x(v), y(v), z(v), w(v) {}
	constexpr explicit Tuple4(T x_, T y_, T z_, T w_) noexcept: x(x_), y(y_), z(z_), w(w_) {}

	constexpr void set(const Tuple4<T>& v) noexcept { *this = v; }
	constexpr void set(T x_, T y_, T z_, T w_) noexcept { x = x_; y = y_; z = z_; w = w_; }
	constexpr void setAll(const T& v)  { set(v,v,v,v); }
	constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Tuple4({}, {}, {}, {})", x, y, z, w);
	}
}; // Tuple4

using Tuple4f = Tuple4<float>;
using Tuple4d = Tuple4<double>;
using Tuple4i = Tuple4<i32>;

AXE_FORMATTER_T(class T, Tuple4<T>)

} // namespace axe