#pragma once

#include "Vec2.h"

namespace axe {

template<class T>
class Margin2 {
	using This = Margin2<T>;
public:
	using ElementType = T;
	using Vec2		  = Vec2<T>;

	static const size_t kElementCount = 4;

	union {
		struct { T top, right, bottom, left; }; //as same as CSS Margin order
		T data[kElementCount];
	};

	constexpr explicit	Margin2() = default;
	constexpr			Margin2(T v)									noexcept : top(v), right(v), bottom(v), left(v) {}
	constexpr			Margin2(T x, T y)								noexcept { set(x, y); }
	constexpr			Margin2(T top_, T right_, T bottom_, T left_)	noexcept : top(top_), right(right_), bottom(bottom_), left(left_) {}

	constexpr void set(T v)									noexcept { top = v; right = v; bottom = v; left = v; }
	constexpr void set(T x, T y)								noexcept { top = y; right = x; bottom = y; left = x; }
	constexpr void set(T top_, T right_, T bottom_, T left_)	noexcept { top = top_; right = right_; bottom = bottom_; left = left_; }

	AXE_NODISCARD constexpr Vec2 topLeft		() const noexcept { return Vec2(left,  top   ); }
	AXE_NODISCARD constexpr Vec2 topRight	() const noexcept { return Vec2(right, top   ); }
	AXE_NODISCARD constexpr Vec2 bottomLeft	() const noexcept { return Vec2(left,  bottom); }
	AXE_NODISCARD constexpr Vec2 bottomRight	() const noexcept { return Vec2(right, bottom); }

	AXE_NODISCARD constexpr T		totalX	() const noexcept { return left + right;  }
	AXE_NODISCARD constexpr T		totalY	() const noexcept { return top  + bottom; }
	AXE_NODISCARD constexpr Vec2	total	() const noexcept { return Vec2(totalX(), totalY()); }

	AXE_NODISCARD constexpr Margin2 inflate(T x, T y) const { return Margin2(top - y, right + x, bottom + y, left - x); }

	constexpr bool operator == (const This& r) const { return top == r.top && right == r.right && bottom == r.bottom && left == r.left; }
	constexpr bool operator != (const This& r) const { return !(this->operator==(r)); }

	AXE_NODISCARD constexpr Margin2 operator-() const { return Margin2(-top, -right, -bottom, -left); }

	AXE_NODISCARD constexpr Margin2 operator + (const T& r) const { return Margin2(top + r, right + r, bottom + r, left + r); }
	AXE_NODISCARD constexpr Margin2 operator - (const T& r) const { return Margin2(top - r, right - r, bottom - r, left - r); }
	AXE_NODISCARD constexpr Margin2 operator * (const T& r) const { return Margin2(top * r, right * r, bottom * r, left * r); }
	AXE_NODISCARD constexpr Margin2 operator / (const T& r) const { return Margin2(top / r, right / r, bottom / r, left / r); }

	AXE_NODISCARD constexpr Margin2 operator + (const Vec2& r) const { return Margin2(top + r.y, right + r.x, bottom + r.y, left + r.x); }
	AXE_NODISCARD constexpr Margin2 operator - (const Vec2& r) const { return Margin2(top - r.y, right - r.x, bottom - r.y, left - r.x); }
	AXE_NODISCARD constexpr Margin2 operator * (const Vec2& r) const { return Margin2(top * r.y, right * r.x, bottom * r.y, left * r.x); }
	AXE_NODISCARD constexpr Margin2 operator / (const Vec2& r) const { return Margin2(top / r.y, right / r.x, bottom / r.y, left / r.x); }

	AXE_NODISCARD constexpr Margin2 operator + (const Margin2& r) const { return Margin2(top + r.top, right + r.right, bottom + r.bottom, left + r.left); }
	AXE_NODISCARD constexpr Margin2 operator - (const Margin2& r) const { return Margin2(top - r.top, right - r.right, bottom - r.bottom, left - r.left); }
	AXE_NODISCARD constexpr Margin2 operator * (const Margin2& r) const { return Margin2(top * r.top, right * r.right, bottom * r.bottom, left * r.left); }
	AXE_NODISCARD constexpr Margin2 operator / (const Margin2& r) const { return Margin2(top / r.top, right / r.right, bottom / r.bottom, left / r.left); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Margin2(\n  top={},\n  right={},\n  bottom={},\n  left={})", top, right, bottom, left);
	}
}; // Margin2

using Margin2i = Margin2<int>;
using Margin2f = Margin2<float>;

AXE_FORMATTER_T(class T, Margin2<T>)

} // namespace axe