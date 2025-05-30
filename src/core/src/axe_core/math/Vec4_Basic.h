#pragma once

#include "Vec3.h"

namespace axe {

template<class T> using Vec4_Basic_Data = Tuple4<T>;

template<class T, class DATA = Vec4_Basic_Data<T> >
struct Vec4_Basic : public DATA {
	using Vec4 = Vec4_Basic;

	using DATA::x; // require this on gcc/clang, otherwise the fullname `Data::x` is needed instead of `x`
	using DATA::y;
	using DATA::z;
	using DATA::w;
	using DATA::data;

	using Vec3 = Vec3<T>;
	using Vec2 = Vec2<T>;

	axeTuple_InterfaceFunctions_Impl(Vec4_Basic, typename DATA::ElementType, 4)

	AXE_INLINE constexpr static		This s_zero()				{ return This(0,0,0,0); }
	AXE_INLINE constexpr static		This s_one()				{ return This(1,1,1,1); }
	AXE_INLINE constexpr static		This s_xy01(const Vec2& v)	{ return This(v.x, v.y, T(0), T(1)); }
	AXE_INLINE constexpr static		This s_xyz1(const Vec3& v)	{ return This(v.x, v.y, v.z,  T(1)); }

	AXE_INLINE constexpr static		This s_inf()				{ auto v = Math::inf<T>(); return This(v); }

	AXE_INLINE constexpr explicit	Vec4(T v)								{ DATA::setAll(v); }
	AXE_INLINE constexpr			Vec4(T x_, T y_, T z_, T w_)			{ DATA::set( x_,  y_,  z_, w_); }
	AXE_INLINE constexpr			Vec4(const Vec3& v, const T& w_)		{ DATA::set(v.x, v.y, v.z, w_); }
	AXE_INLINE constexpr			Vec4(const Tuple3<T>& v, const T& w_)	{ DATA::set(v.x, v.y, v.z, w_); }
	AXE_INLINE constexpr			Vec4(const Tuple4<T>& v)				{ DATA::set(v); }

	AXE_INLINE constexpr void setToDefaultValue()			{ DATA::set(0,0,0,1); }
	AXE_INLINE constexpr bool isAll(const T& v)		const	{ return equals(This(v)); }

	AXE_INLINE constexpr bool equals (const This& r, const T& epsilon = Math::epsilon<T>()) const;
	AXE_INLINE constexpr bool equals0(				 const T& epsilon = Math::epsilon<T>()) const;

	AXE_INLINE constexpr This operator - ()				  const { return This(-x, -y, -z, -w); }

	AXE_INLINE			 This operator + (const This& r) const { return This(x + r.x, y + r.y, z + r.z, w + r.w); }
	AXE_INLINE			 This operator - (const This& r) const { return This(x - r.x, y - r.y, z - r.z, w - r.w); }
	AXE_INLINE			 This operator * (const This& r) const { return This(x * r.x, y * r.y, z * r.z, w * r.w); }
	AXE_INLINE			 This operator / (const This& r) const { return This(x / r.x, y / r.y, z / r.z, w / r.w); }

	AXE_INLINE			 This operator + (T s) const { return This(x + s, y + s, z + s, w + s); }
	AXE_INLINE			 This operator - (T s) const { return This(x - s, y - s, z - s, w - s); }
	AXE_INLINE			 This operator * (T s) const { return This(x * s, y * s, z * s, w * s); }
	AXE_INLINE			 This operator / (T s) const { return This(x / s, y / s, z / s, w / s); }

	AXE_INLINE			 void operator += (const This& r) { x += r.x; y += r.y; z += r.z; w += r.w; }
	AXE_INLINE			 void operator -= (const This& r) { x -= r.x; y -= r.y; z -= r.z; w -= r.w; }
	AXE_INLINE			 void operator *= (const This& r) { x *= r.x; y *= r.y; z *= r.z; w *= r.w; }
	AXE_INLINE			 void operator /= (const This& r) { x /= r.x; y /= r.y; z /= r.z; w /= r.w; }

	AXE_INLINE			 void operator += (T s) { x += s; y += s; z += s; w += s; }
	AXE_INLINE			 void operator -= (T s) { x -= s; y -= s; z -= s; w -= s; }
	AXE_INLINE			 void operator *= (T s) { x *= s; y *= s; z *= s; w *= s; }
	AXE_INLINE			 void operator /= (T s) { x /= s; y /= s; z /= s; w /= s; }

	AXE_INLINE			 bool operator == (const This& r) const { return x == r.x && y == r.y && z == r.z && w == r.w; }
	AXE_INLINE			 bool operator != (const This& r) const { return x != r.x || y != r.y || z != r.z || w != r.w; }

	AXE_NODISCARD AXE_INLINE constexpr Vec2 xy () const { return Vec2(x,y); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 xz () const { return Vec2(x,z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec2 yz () const { return Vec2(y,z); }
	AXE_NODISCARD AXE_INLINE constexpr Vec3 xyz() const { return Vec3(x,y,z); }

	AXE_NODISCARD AXE_INLINE Vec3 homogenize()	const { return (*this / w).xyz(); };
	AXE_NODISCARD AXE_INLINE Vec3 toVec3()		const { return homogenize(); };

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "({}, {}, {}, {})", x, y, z, w);
	}

				  AXE_INLINE constexpr void operator=		(const Tuple4<T>& v) { DATA::set(v.x, v.y, v.z, v.w); }
	AXE_NODISCARD AXE_INLINE constexpr Tuple4<T> toTuple	() const			 { return Tuple4<T>(x, y, z, w); }
				  AXE_INLINE constexpr operator Tuple4<T>	() const			 { return toTuple(); }

	template<class V> AXE_INLINE constexpr
	static Vec4 s_cast(const V& v) { return Vec4(static_cast<ElementType>(v.x),
												 static_cast<ElementType>(v.y),
												 static_cast<ElementType>(v.z),
												 static_cast<ElementType>(v.w));
	}
};

using Vec4f_Basic = Vec4_Basic<float>;
using Vec4d_Basic = Vec4_Basic<double>;

AXE_FORMATTER_T( AXE_ARGS(class T, class DATA), Vec4_Basic< AXE_ARGS(T, DATA) >)

#if 0
#pragma mark ------------------- instance functions -------------------
#endif

template<class T, class DATA> AXE_INLINE constexpr
bool Vec4_Basic<T, DATA>::equals(const This& r, const T& epsilon) const {
	return Math::equals(x, r.x, epsilon)
		&& Math::equals(y, r.y, epsilon)
		&& Math::equals(z, r.z, epsilon)
		&& Math::equals(w, r.w, epsilon);
}

template<class T, class DATA> AXE_INLINE constexpr
bool Vec4_Basic<T, DATA>::equals0(const T& epsilon) const {
	return Math::equals0(x, epsilon)
		&& Math::equals0(y, epsilon)
		&& Math::equals0(z, epsilon)
		&& Math::equals0(w, epsilon);
}

#if 0
#pragma mark ------------------- global functions -------------------
#endif

namespace Math {

template<class T, class DATA> AXE_INLINE
void sincos(const Vec4_Basic<T, DATA>& th, Vec4_Basic<T, DATA>& outSin, Vec4_Basic<T, DATA>& outCos) {
	Math::sincos(th.x, outSin.x, outCos.x);
	Math::sincos(th.y, outSin.y, outCos.y);
	Math::sincos(th.z, outSin.z, outCos.z);
	Math::sincos(th.w, outSin.w, outCos.w);
}

template<class T, class DATA> AXE_INLINE
Vec4_Basic<T, DATA> min(const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::min(a.x, b.x),
								Math::min(a.y, b.y),
								Math::min(a.z, b.z),
								Math::min(a.w, b.w));
}

template<class T, class DATA> AXE_INLINE
Vec4_Basic<T, DATA> max(const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::max(a.x, b.x),
								Math::max(a.y, b.y),
								Math::max(a.z, b.z),
								Math::max(a.w, b.w));
}

template<class T, class DATA> AXE_INLINE
Vec4_Basic<T, DATA> clamp(const Vec4_Basic<T, DATA>& v, const Vec4_Basic<T, DATA>& a, const Vec4_Basic<T, DATA>& b) {
	return Vec4_Basic<T, DATA>(	Math::clamp(v.x, a.x, b.x),
								Math::clamp(v.y, a.y, b.y),
								Math::clamp(v.z, a.z, b.z),
								Math::clamp(v.w, a.w, b.w));
}

}} // namespace axe/Math
