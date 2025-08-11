#pragma once

#include "Tuple2.h"

namespace axe {

template<class T>
class Tuple3 {
public:
	axeTuple_InterfaceFunctions_Impl(Tuple3, T, 3)
	union {
		struct { T x, y, z; };
		T data[kElementCount];
	};


	constexpr explicit Tuple3(T v)			    noexcept : x(v), y(v), z(v) {}
	constexpr explicit Tuple3(T x_, T y_, T z_) noexcept : x(x_), y(y_), z(z_) {}

	constexpr void set(const Tuple3<T>& v) { *this = v; }
	constexpr void set(T x_, T y_, T z_) {
		x = x_; y = y_; z = z_;
	}
	constexpr void setAll(const T& v)  { set(v,v,v); }
	constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Tuple3({}, {}, {})", x, y, z);
	}
}; // Tuple3

using Tuple3f = Tuple3<float>;
using Tuple3d = Tuple3<double>;
using Tuple3i = Tuple3<i32>;

AXE_FORMATTER_T(class T, Tuple3<T>)

} // namespace axe