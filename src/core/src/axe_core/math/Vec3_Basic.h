#pragma once

#include "Vec2.h"

namespace axe {

template<class T> using Vec3_Basic_Data = Tuple3<T>;

template<class T, class DATA = Vec3_Basic_Data<T> >
struct Vec3_Basic : public DATA {
	using Vec3 = Vec3_Basic;
	using Vec2 = Vec2<T>;

	axeTuple_InterfaceFunctions_Impl(Vec3_Basic, typename DATA::ElementType, 3)

	using DATA::x; // require this on gcc/clang, otherwise the fullname `DATA::x` is needed instead of `x`
	using DATA::y;
	using DATA::z;
	using DATA::data;

	AXE_INLINE static constexpr This s_zero()		{ return This( 0, 0, 0); }
	AXE_INLINE static constexpr This s_one()		{ return This( 1, 1, 1); }

	AXE_INLINE static constexpr This s_forward()	{ return This( 0, 0, 1); }
	AXE_INLINE static constexpr This s_back()		{ return This( 0, 0,-1); }
	AXE_INLINE static constexpr This s_up()			{ return This( 0, 1, 0); }
	AXE_INLINE static constexpr This s_down()		{ return This( 0,-1, 0); }
	AXE_INLINE static constexpr This s_right()		{ return This( 1 ,0, 0); }
	AXE_INLINE static constexpr This s_left()		{ return This(-1 ,0, 0); }

	AXE_INLINE static constexpr This s_inf()		{ auto v = Math::inf<T>(); return This(v); }

	AXE_INLINE static constexpr This s_xy0(const Vec2& v) { return This(v.x, v.y, T(0)); }
	AXE_INLINE static constexpr This s_xy1(const Vec2& v) { return This(v.x, v.y, T(1)); }
	AXE_INLINE static constexpr This s_x0y(const Vec2& v) { return This(v.x, T(0), v.y); }
	AXE_INLINE static constexpr This s_x1y(const Vec2& v) { return This(v.x, T(1), v.y); }

	AXE_INLINE explicit	constexpr Vec3(T v)								{ DATA::setAll(v); }
	AXE_INLINE			constexpr Vec3(T x_, T y_, T z_)				{ DATA::set(x_,  y_,  z_); }
	AXE_INLINE			constexpr Vec3(const Vec2& v, const T& z_)		{ DATA::set(v.x, v.y, z_); }
	AXE_INLINE			constexpr Vec3(const Tuple2<T>& v, const T& z_) { DATA::set(v.x, v.y, z_); }
	AXE_INLINE			constexpr Vec3(const Tuple3<T>& v)				{ DATA::set(v); }

	AXE_INLINE constexpr void setToDefaultValue()		{ DATA::set(0,0,0); }
	AXE_INLINE constexpr bool isAll (const T& v) const	{ return equals(This(v)); }

	AXE_INLINE constexpr bool equals (const This& r, const T& epsilon = Math::epsilon<T>()) const;
	AXE_INLINE constexpr bool equals0(               const T& epsilon = Math::epsilon<T>()) const;

	AXE_INLINE constexpr This operator - ()				   const { return This(-x, -y, -z); }

	AXE_INLINE			 This operator + (const This &	r) const { return This(x + r.x, y + r.y, z + r.z); }
	AXE_INLINE			 This operator - (const This &	r) const { return This(x - r.x, y - r.y, z - r.z); }
	AXE_INLINE			 This operator * (const This &	r) const { return This(x * r.x, y * r.y, z * r.z); }
	AXE_INLINE			 This operator / (const This &	r) const { return This(x / r.x, y / r.y, z / r.z); }

	AXE_INLINE			 This operator + (T	s) const { return This(x + s, y + s, z + s); }
	AXE_INLINE			 This operator - (T	s) const { return This(x - s, y - s, z - s); }
	AXE_INLINE			 This operator * (T	s) const { return This(x * s, y * s, z * s); }
	AXE_INLINE			 This operator / (T	s) const { return This(x / s, y / s, z / s); }

	AXE_INLINE			 void operator += (const This & r) { x += r.x; y += r.y; z += r.z; }
	AXE_INLINE			 void operator -= (const This & r) { x -= r.x; y -= r.y; z -= r.z; }
	AXE_INLINE			 void operator *= (const This & r) { x *= r.x; y *= r.y; z *= r.z; }
	AXE_INLINE			 void operator /= (const This & r) { x /= r.x; y /= r.y; z /= r.z; }

	AXE_INLINE			 void operator += (T s) { x += s; y += s; z += s; }
	AXE_INLINE			 void operator -= (T s) { x -= s; y -= s; z -= s; }
	AXE_INLINE			 void operator *= (T s) { x *= s; y *= s; z *= s; }
	AXE_INLINE			 void operator /= (T s) { x /= s; y /= s; z /= s; }

	AXE_INLINE constexpr bool operator == (const This & r) const { return x == r.x && y == r.y && z == r.z; }
	AXE_INLINE constexpr bool operator != (const This & r) const { return x != r.x || y != r.y || z != r.z; }

	AXE_NODISCARD AXE_INLINE This	cross	(const This& v) const { return This(y*v.z - z*v.y, z*v.x - x*v.z,x*v.y - y*v.x); }
	AXE_NODISCARD AXE_INLINE T		dot		(const This& v) const { return (x*v.x) + (y*v.y) + (z*v.z); }

	AXE_NODISCARD AXE_INLINE T		magnitude		() const { T m = sqrMagnitude(); return Math::equals0(m) ? T(0) : Math::sqrt(m); }
	AXE_NODISCARD AXE_INLINE T		sqrMagnitude	() const { return dot(*this); }

	AXE_NODISCARD AXE_INLINE T		length			() const { return magnitude(); }
	AXE_NODISCARD AXE_INLINE T		sqrLength		() const { return sqrMagnitude(); }

	AXE_NODISCARD AXE_INLINE T		distance		(const This &r) const { return (*this - r).length();    }
	AXE_NODISCARD AXE_INLINE T		sqrDistance		(const This &r) const { return (*this - r).sqrLength(); }

	AXE_NODISCARD AXE_INLINE constexpr This normal	 ()	const { T m = magnitude(); return Math::equals0(m) ? s_zero() : (*this / m); }
				  AXE_INLINE constexpr void normalize()		  { *this = normal(); }

	AXE_NODISCARD AXE_INLINE constexpr Vec3 xyz() const { return Vec3(x, y, z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 xzy() const { return Vec3(x, z, y); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 yxz() const { return Vec3(y, x, z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 yzx() const { return Vec3(y, z, x); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 zxy() const { return Vec3(z, x, y); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 zyx() const { return Vec3(z, y, x); }

	AXE_NODISCARD AXE_INLINE constexpr Vec2 xy () const { return Vec2(x, y); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 xz () const { return Vec2(x, z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 yx () const { return Vec2(y, x); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 yz () const { return Vec2(y, z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 zx () const { return Vec2(z, x); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 zy () const { return Vec2(z, y); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {})", x, y, z);
	}

				  AXE_INLINE constexpr void operator=		(const Tuple3<T>& v) { DATA::set(v.x, v.y, v.z); }
	AXE_NODISCARD AXE_INLINE constexpr Tuple3<T> toTuple	() const			 { return Tuple3<T>(x, y, z); }
				  AXE_INLINE constexpr operator Tuple3<T>	() const			 { return toTuple(); }

	template<class V> AXE_INLINE constexpr
	static This s_cast(const V& v) { return This(static_cast<ElementType>(v.x),
												 static_cast<ElementType>(v.y),
												 static_cast<ElementType>(v.z));
	}
};

using Vec3f_Basic = Vec3_Basic<float>;
using Vec3d_Basic = Vec3_Basic<double>;

// work around for comma
// AXE_FORMATTER_T(class T AXE_COMMA class DATA, Vec3_Basic<T AXE_COMMA DATA>)

// another work around for comma
AXE_FORMATTER_T( AXE_ARGS(class T, class DATA), Vec3_Basic< AXE_ARGS(T, DATA) >)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif

template<class T, class DATA> AXE_INLINE constexpr
bool Vec3_Basic<T, DATA>::equals(const This& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon);
}

template<class T, class DATA> AXE_INLINE constexpr
bool Vec3_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon);
}

#if 0
#pragma mark ------------------- global functions -------------------
#endif

namespace Math {

template<class T, class DATA> AXE_INLINE
void sincos(const Vec3_Basic<T, DATA>& th, Vec3_Basic<T, DATA>& outSin, Vec3_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
	Math::sincos(th.z, outSin.z, outCos.z);
}

template<class T, class DATA> AXE_INLINE
Vec3_Basic<T, DATA> min(const Vec3_Basic<T, DATA>& a, const Vec3_Basic<T, DATA>& b) {
	return Vec3_Basic<T, DATA>(	Math::min(a.x, b.x),
								Math::min(a.y, b.y),
								Math::min(a.z, b.z));
}

template<class T, class DATA> AXE_INLINE
Vec3_Basic<T, DATA> max(const Vec3_Basic<T, DATA>& a, const Vec3_Basic<T, DATA>& b) {
	return Vec3_Basic<T, DATA>(	Math::max(a.x, b.x),
								Math::max(a.y, b.y),
								Math::max(a.z, b.z));
}

template<class T, class DATA> AXE_INLINE
Vec3_Basic<T, DATA> clamp(const Vec3_Basic<T, DATA>& v, const Vec3_Basic<T, DATA>& a, const Vec3_Basic<T, DATA>& b) {
	return Vec3_Basic<T, DATA>(	Math::clamp(v.x, a.x, b.x),
								Math::clamp(v.y, a.y, b.y),
								Math::clamp(v.z, a.z, b.z));
}

}} // namespace axe/Math