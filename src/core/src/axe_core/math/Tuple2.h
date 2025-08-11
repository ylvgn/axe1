#pragma once

#include <axe_core/base/Error.h>
#include <axe_core/log/Log.h>
#include <axe_core/base/Reflection.h>
#include "Math.h"

#define axeTuple_InterfaceFunctions_Impl(AXE_T, ELEMENT_TYPE, COUNT) \
private: \
	using This = AXE_T; \
public: \
	using ElementType = ELEMENT_TYPE; \
	static const size_t kElementCount = COUNT; \
	\
	constexpr explicit This() = default; \
	\
			ElementType& operator[]	(int i)			{ return at(i); } \
	const	ElementType& operator[]	(int i)	const	{ return at(i); } \
	\
			ElementType& at			(int i)			{ s_checkBound(i); return data[i]; } \
	const	ElementType& at			(int i)	const	{ s_checkBound(i); return data[i]; } \
	\
			ElementType& unsafe_at	(int i)			{ return data[i]; } \
	const	ElementType& unsafe_at	(int i)	const	{ return data[i]; } \
	\
			Span<		ElementType> span()			{ return Span<		ElementType>(&x, kElementCount); } \
			Span<const	ElementType> span() const	{ return Span<const ElementType>(&x, kElementCount); } \
	\
	template<class V> constexpr void setByCast(const V& v) { *this = s_cast(v); } \
private: \
	static constexpr bool s_inBound		(int i)		{ return i >= 0 && i < kElementCount; } \
	static constexpr void s_checkBound	(int i)		{ if (!s_inBound(i)) throw std::out_of_range(__FILE__); } \
public: \
//----


namespace axe {

template<class T>
class Tuple2 {
public:
	axeTuple_InterfaceFunctions_Impl(Tuple2, T, 2)

	union {
		struct { T x, y; };
		T data[kElementCount];
	};

	constexpr explicit Tuple2(T v)		  noexcept : x(v), y(v) {}
	constexpr explicit Tuple2(T x_, T y_) noexcept : x(x_), y(y_) {}

	constexpr void set(const Tuple2<T>& v) { *this = v; }
	constexpr void set(T x_, T y_) {
		x = x_; y = y_;
	}

	constexpr void setAll(const T& v)  { set(v,v); }
	constexpr void setToDefaultValue() { setAll(0); }

	void onFormat(fmt::format_context& ctx) const {
		fmt::format_to(ctx.out(), "Tuple2({}, {})", x, y);
	}
}; // Tuple2

using Tuple2f = Tuple2<float>;
using Tuple2d = Tuple2<double>;
using Tuple2i = Tuple2<i32>;

AXE_FORMATTER_T(class T, Tuple2<T>)

} // namespace axe