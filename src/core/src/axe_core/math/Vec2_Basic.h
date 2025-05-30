#pragma once

#include "Tuple4.h"

namespace axe {

template<class T> using Vec2_Basic_Data = Tuple2<T>;

template<class T, class DATA = Vec2_Basic_Data<T> >
struct Vec2_Basic : public DATA {
	using Vec2 = Vec2_Basic;

	axeTuple_InterfaceFunctions_Impl(Vec2_Basic, typename DATA::ElementType, 2)

	using DATA::x; // require this on gcc/clang, otherwise the fullname `DATA::x` is needed instead of `x`
	using DATA::y;
	using DATA::data;

	AXE_INLINE constexpr static   This s_zero()		{ return This( 0, 0); }
	AXE_INLINE constexpr static   This s_one()		{ return This( 1, 1); }

	AXE_INLINE constexpr static   This s_up()		{ return This( 0, 1); }
	AXE_INLINE constexpr static   This s_down()		{ return This( 0,-1); }
	AXE_INLINE constexpr static   This s_right()	{ return This( 1, 0); }
	AXE_INLINE constexpr static   This s_left()		{ return This(-1, 0); }

	AXE_INLINE constexpr static   This s_inf()		{ auto v = Math::inf<T>(); return This(v); }

	AXE_INLINE constexpr explicit Vec2(T v)					{ DATA::setAll(v); }
	AXE_INLINE constexpr	      Vec2(T x_, T y_)			{ DATA::set(x_, y_); }
	AXE_INLINE constexpr	      Vec2(const Tuple2<T>& v)	{ DATA::set(v); }

	AXE_INLINE constexpr void setToDefaultValue()			{ DATA::set(0,0); }
	AXE_INLINE constexpr bool isAll(const T& v)		 const  { return equals(This(v)); }

	AXE_INLINE constexpr bool equals (const This& r, const T& epsilon = Math::epsilon<T>()) const;
	AXE_INLINE constexpr bool equals0(				 const T& epsilon = Math::epsilon<T>()) const;

	AXE_INLINE constexpr This operator - ()				 const { return This(-x, -y); }

	AXE_INLINE			 This operator + (const This& r) const { return This(x + r.x, y + r.y); }
	AXE_INLINE			 This operator - (const This& r) const { return This(x - r.x, y - r.y); }
	AXE_INLINE			 This operator * (const This& r) const { return This(x * r.x, y * r.y); }
	AXE_INLINE			 This operator / (const This& r) const { return This(x / r.x, y / r.y); }

	AXE_INLINE			 This operator + (T s) const { return This(x + s, y + s); }
	AXE_INLINE			 This operator - (T s) const { return This(x - s, y - s); }
	AXE_INLINE			 This operator * (T s) const { return This(x * s, y * s); }
	AXE_INLINE			 This operator / (T s) const { return This(x / s, y / s); }

	AXE_INLINE			 void operator += (const This& r) { x += r.x; y += r.y; }
	AXE_INLINE			 void operator -= (const This& r) { x -= r.x; y -= r.y; }
	AXE_INLINE			 void operator *= (const This& r) { x *= r.x; y *= r.y; }
	AXE_INLINE			 void operator /= (const This& r) { x /= r.x; y /= r.y; }

	AXE_INLINE			 void operator += (T s) { x += s; y += s; }
	AXE_INLINE			 void operator -= (T s) { x -= s; y -= s; }
	AXE_INLINE			 void operator *= (T s) { x *= s; y *= s; }
	AXE_INLINE			 void operator /= (T s) { x /= s; y /= s; }

	AXE_INLINE constexpr bool operator == (const This& r) const { return x == r.x && y == r.y; }
	AXE_INLINE constexpr bool operator != (const This& r) const { return x != r.x || y != r.y; }

	AXE_NODISCARD AXE_INLINE T		magnitude()		const { return Math::sqrt(sqrMagnitude()); }
	AXE_NODISCARD AXE_INLINE T		sqrMagnitude()	const { return x * x + y * y; }

	AXE_NODISCARD AXE_INLINE T		length()	const { return magnitude(); }
	AXE_NODISCARD AXE_INLINE T		sqrLength() const { return sqrMagnitude(); }

	AXE_NODISCARD AXE_INLINE T		distance	(const This& r)	const { return (*this - r).length(); }
	AXE_NODISCARD AXE_INLINE T		sqrDistance	(const This& r)	const { return (*this - r).sqrLength(); }

	AXE_NODISCARD AXE_INLINE constexpr This	yx() const { return This(y, x); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {})", x, y);
	}

				  AXE_INLINE constexpr void operator=		(const Tuple2<T>& v) { DATA::set(v.x, v.y); }
	AXE_NODISCARD AXE_INLINE constexpr Tuple2<T> toTuple	() const			 { return Tuple2<T>(x, y); }
				  AXE_INLINE constexpr operator Tuple2<T>	() const			 { return toTuple(); }

	template<class V> AXE_INLINE constexpr
	static This s_cast(const V& v) { return This(static_cast<ElementType>(v.x),
												 static_cast<ElementType>(v.y));
	}

#if AXE_OS_WINDOWS
	AXE_INLINE constexpr explicit Vec2(const POINT& v) { DATA::set((T)v.x, (T)v.y); }
#endif // AXE_OS_WINDOWS
};

using Vec2f_Basic = Vec2_Basic<float>;
using Vec2d_Basic = Vec2_Basic<double>;

AXE_FORMATTER_T( AXE_ARGS(class T, class DATA), Vec2_Basic< AXE_ARGS(T, DATA) >)


#if 0
#pragma mark ------------------- instance functions -------------------
#endif

template<class T, class DATA> AXE_INLINE constexpr
bool Vec2_Basic<T, DATA>::equals(const This& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon);
}

template<class T, class DATA> AXE_INLINE constexpr
bool Vec2_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon);
}

#if 0
#pragma mark ------------------- global functions -------------------
#endif

namespace Math {

template<class T, class DATA> AXE_INLINE
void sincos(const Vec2_Basic<T, DATA>& th, Vec2_Basic<T, DATA>& outSin, Vec2_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
}

template<class T, class DATA> AXE_INLINE
Vec2_Basic<T, DATA> min(const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::min(a.x, b.x),
								Math::min(a.y, b.y));
}

template<class T, class DATA> AXE_INLINE
Vec2_Basic<T, DATA> max(const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::max(a.x, b.x),
								Math::max(a.y, b.y));
}

template<class T, class DATA> AXE_INLINE
Vec2_Basic<T, DATA> clamp(const Vec2_Basic<T, DATA>& v, const Vec2_Basic<T, DATA>& a, const Vec2_Basic<T, DATA>& b) {
	return Vec2_Basic<T, DATA>(	Math::clamp(v.x, a.x, b.x),
								Math::clamp(v.y, a.y, b.y));
}

}} // namespace axe/Math