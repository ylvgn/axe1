#pragma once

#include "Margin2.h"

namespace axe {

template<class T>
class Rect2 {
	using This = Rect2<T>;
public:
	using ElementType = T;
	using Vec2		  = Vec2<T>;
	using Margin2	  = Margin2<T>;

	static const size_t kElementCount = 4;

	union {
		struct { T x, y, w, h; };
		struct { Vec2 pos, size; };
		T data[kElementCount];
	};

	constexpr explicit Rect2() = default;
	constexpr explicit Rect2(T v)									noexcept : x(v), y(v), w(v), h(v) {}
	constexpr		   Rect2(T x_, T y_, T w_, T h_)				noexcept : x(x_), y(y_), w(w_), h(h_) {}
	constexpr		   Rect2(const Vec2& pos_, const Vec2& size_)	noexcept : pos(pos_), size(size_) {}

	constexpr void set(T v)									noexcept { x = v; y = v; w = v; h = v; }
	constexpr void set(T x_, T y_, T w_, T h_)				noexcept { x = x_; y = y_; w = w_; h = h_; }
	constexpr void set(const Vec2& pos_, const Vec2& size_) noexcept { pos = pos_; size = size_; }

	AXE_NODISCARD constexpr Vec2	 center() const noexcept { return pos + size * T(0.5); }
	AXE_NODISCARD constexpr T		xCenter() const noexcept { return x + w * T(0.5); }
	AXE_NODISCARD constexpr T		yCenter() const noexcept { return y + h * T(0.5); }

	AXE_NODISCARD constexpr T		xMin() const noexcept { return x; }
	AXE_NODISCARD constexpr T		yMin() const noexcept { return y; }
	AXE_NODISCARD constexpr T		xMax() const noexcept { return x + w; }
	AXE_NODISCARD constexpr T		yMax() const noexcept { return y + h; }

	AXE_NODISCARD constexpr Vec2	xMinYMin	() const noexcept { return Vec2(xMin(), yMin()); }
	AXE_NODISCARD constexpr Vec2	xMaxYMin	() const noexcept { return Vec2(xMax(), yMin()); }
	AXE_NODISCARD constexpr Vec2	xMinYMax	() const noexcept { return Vec2(xMin(), yMax()); }
	AXE_NODISCARD constexpr Vec2	xMaxYMax	() const noexcept { return Vec2(xMax(), yMax()); }

	AXE_NODISCARD constexpr Vec2	topLeft		() const noexcept { return xMinYMin(); }
	AXE_NODISCARD constexpr Vec2	topRight	() const noexcept { return xMaxYMin(); }
	AXE_NODISCARD constexpr Vec2	bottomLeft	() const noexcept { return xMinYMax(); }
	AXE_NODISCARD constexpr Vec2	bottomRight	() const noexcept { return xMaxYMax(); }

	AXE_NODISCARD constexpr Rect2 offset(const Vec2& v)	   const { return Rect2(pos + v, size); }
	AXE_NODISCARD constexpr Rect2 expand(const Margin2& m) const { return Rect2(x - m.left, y - m.top, w + (m.left + m.right), h + (m.top + m.bottom)); }

	AXE_NODISCARD constexpr Rect2 expandTop		(T v) const { return expand({ v, 0, 0, 0 }); }
	AXE_NODISCARD constexpr Rect2 expandRight	(T v) const { return expand({ 0, v, 0, 0 }); }
	AXE_NODISCARD constexpr Rect2 expandBottom	(T v) const { return expand({ 0, 0, v, 0 }); }
	AXE_NODISCARD constexpr Rect2 expandLeft	(T v) const { return expand({ 0, 0, 0, v }); }

	AXE_NODISCARD constexpr T area		 () const { return w * h; }
	AXE_NODISCARD constexpr T perimeter	 () const { return w + w + h + h; }

	AXE_NODISCARD constexpr bool	 containsPoint	(const Vec2& pt) const;
	AXE_NODISCARD constexpr bool	 isIntersected	(const Rect2& r) const;
	AXE_NODISCARD constexpr Rect2<T> intersects		(const Rect2& r) const;
	AXE_NODISCARD constexpr Rect2<T> unionWith		(const Rect2& r) const;

	constexpr bool operator == (const This& r) const {
		AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wfloat-equal")
		return x == r.x && y == r.y && w == r.w && h == r.h;
		AXE_GCC_WARNING_POP()
	}
	constexpr bool operator != (const This& r) const { return !(this->operator==(r)); }

	AXE_NODISCARD constexpr Rect2 operator-() const { return Rect2(-x, -y, -w, -h); }

	AXE_NODISCARD constexpr Rect2 operator + (const Vec2& v) const { return offset(v); }
	AXE_NODISCARD constexpr Rect2 operator - (const Vec2& v) const { return offset(-v); }

	AXE_NODISCARD constexpr Rect2 operator + (const Margin2& m) const { return expand(m); }
	AXE_NODISCARD constexpr Rect2 operator - (const Margin2& m) const { return expand(-m); }

				  constexpr void  operator+= (const Margin2& m) { *this = expand( m); }
				  constexpr void  operator-= (const Margin2& m) { *this = expand(-m); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, w, h);
	}

#if AXE_OS_WINDOWS
	explicit constexpr Rect2(const ::RECT& r) { set(r); }

	constexpr void set(const ::RECT& src) {
		set(T(src.left),
			T(src.top),
			T(src.right  - src.left),
			T(src.bottom - src.top)
		);
	}

	constexpr ::RECT to_RECT() const {
		::RECT o;
		using DST = decltype(o.left);
		o.left	  = static_cast<DST>(xMin());
		o.top	  = static_cast<DST>(yMin());
		o.right	  = static_cast<DST>(xMax());
		o.bottom  = static_cast<DST>(yMax());
		return o;
	}
#endif // AXE_OS_WINDOWS

}; // Rect2

using Rect2i = Rect2<int>;
using Rect2f = Rect2<float>;

AXE_FORMATTER_T(class T, Rect2<T>)


template<class T> AXE_NODISCARD inline constexpr 
bool Rect2<T>::containsPoint(const Vec2& pt) const {
	return pt.x >= xMin() && pt.x <= xMax() && pt.y >= yMin() && pt.y <= yMax();
}

template<class T> AXE_NODISCARD inline constexpr 
bool Rect2<T>::isIntersected(const Rect2& r) const {
	if (r.x > xMax() || x > r.xMax()) return false;
	if (r.y > yMax() || y > r.yMax()) return false;
	return true;
}

template <class T> AXE_NODISCARD inline constexpr
Rect2<T> Rect2<T>::intersects(const Rect2& r) const {
	if (!isIntersected(r)) return This(0, 0, 0, 0);

	T left	 = Math::max(xMin(), r.xMin());
	T top	 = Math::max(yMin(), r.yMin());
	T right	 = Math::min(xMax(), r.xMax());
	T bottom = Math::min(yMax(), r.yMax());
	return Rect2(left, top, right - left, bottom - top);
}

template <class T> AXE_NODISCARD inline constexpr
Rect2<T> Rect2<T>::unionWith(const Rect2& r) const {
	T left	 = Math::min(xMin(), r.xMin());
	T top	 = Math::min(yMin(), r.yMin());
	T right	 = Math::max(xMax(), r.xMax());
	T bottom = Math::max(yMax(), r.yMax());
	return Rect2(left, top, right - left, bottom - top);
}

} // namespace axe