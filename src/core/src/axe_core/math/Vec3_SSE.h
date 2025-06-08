#pragma once

#include "Vec3_Basic.h"

namespace axe {

template<class T, class M>
struct alignas(alignof(M)) Vec3_SSE_Data {
	axeTuple_InterfaceFunctions_Impl(Vec3_SSE_Data, T, 3)
	using RegisterType	= M;

	union {
		struct { T x, y, z, _unused_padding; };
		T data[kElementCount];
		M _m;
	};

	AXE_INLINE explicit constexpr Vec3_SSE_Data(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
	AXE_INLINE			constexpr Vec3_SSE_Data(const Tuple3<T>& v) : x(x_), y(y_), z(z_) {}

	AXE_INLINE void set(T x_, T y_, T z_)		{ x = x_; y = y_; z = z_; }
	AXE_INLINE void set(const Tuple3<T>& v)		{ set(v.x, v.y, v.z); }
	AXE_INLINE void setAll(const T& v)			{ set(v,v,v); }
};

using Vec3f_SSE_Data = Vec3_SSE_Data< float,  __m128>; // SSE
using Vec3d_SSE_Data = Vec3_SSE_Data<double, __m256d>; // AVX

template<class T> struct Vec3_SSE_Select  { using Data = Vec3_Basic_Data<T>; };
template<> struct Vec3_SSE_Select< float> { using Data = Vec3f_SSE_Data; };
template<> struct Vec3_SSE_Select<double> { using Data = Vec3d_SSE_Data; };

template<class T> using Vec3_SSE = Vec3_Basic<T, typename Vec3_SSE_Select<T>::Data >;

using Vec3f_SSE = Vec3_SSE< float>;
using Vec3d_SSE = Vec3_SSE<double>;

AXE_FORMATTER_T(class T, Vec3_SSE<T>)

AXE_INLINE constexpr Vec3f_SSE Vec3_SSE_make( __m128 m) { Vec3f_SSE o; o._m = m; return o; }
AXE_INLINE constexpr Vec3d_SSE Vec3_SSE_make(__m256d m) { Vec3d_SSE o; o._m = m; return o; }

#if defined(AXE_CPU_ENDIAN_LITTLE)
template<> AXE_INLINE void Vec3f_SSE::set( float x_,  float y_,  float z_) { _m =	 _mm_set_ps(0, z_, y_, x_); }
template<> AXE_INLINE void Vec3d_SSE::set(double x_, double y_, double z_) { _m = _mm256_set_pd(0, z_, y_, x_); }

#elif defined(AXE_CPU_ENDIAN_BIG)
template<> AXE_INLINE void Vec3f_SSE::set( float x_,  float y_,  float z_) { _m =	 _mm_set_ps(x_, y_, z_, 0); }
template<> AXE_INLINE void Vec3d_SSE::set(double x_, double y_, double z_) { _m = _mm256_set_pd(x_, y_, z_, 0); }

#else
	#error
#endif

template<> AXE_INLINE Vec3f_SSE Vec3f_SSE::operator + (const Vec3& r) const { return Vec3_SSE_make(   _mm_add_ps(_m, r._m)); }
template<> AXE_INLINE Vec3f_SSE Vec3f_SSE::operator - (const Vec3& r) const { return Vec3_SSE_make(   _mm_sub_ps(_m, r._m)); }
template<> AXE_INLINE Vec3f_SSE Vec3f_SSE::operator * (const Vec3& r) const { return Vec3_SSE_make(   _mm_mul_ps(_m, r._m)); }
template<> AXE_INLINE Vec3f_SSE Vec3f_SSE::operator / (const Vec3& r) const { return Vec3_SSE_make(   _mm_div_ps(_m, r._m)); }

template<> AXE_INLINE Vec3d_SSE Vec3d_SSE::operator + (const Vec3& r) const { return Vec3_SSE_make(_mm256_add_pd(_m, r._m)); }
template<> AXE_INLINE Vec3d_SSE Vec3d_SSE::operator - (const Vec3& r) const { return Vec3_SSE_make(_mm256_sub_pd(_m, r._m)); }
template<> AXE_INLINE Vec3d_SSE Vec3d_SSE::operator * (const Vec3& r) const { return Vec3_SSE_make(_mm256_mul_pd(_m, r._m)); }
template<> AXE_INLINE Vec3d_SSE Vec3d_SSE::operator / (const Vec3& r) const { return Vec3_SSE_make(_mm256_div_pd(_m, r._m)); }

} // namespace axe