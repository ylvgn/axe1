#pragma once

// cppreference: https://en.cppreference.com/w/cpp/meta.html
//	- keep naming-convention with std
//	- usage: please check test_TypeTraits.cpp

// There are three primary uses of type traits:
//	- optimized operations on some data types.
//	- different logic pathways based on data types.
//	- compile time assertions about data type expectations.


namespace axe {

//+----------------------------------------------+
//|             Type trait constants             |
//+----------------------------------------------+
#if 0
#pragma mark------------ integral_constant --------------
// integral_constant: This is a C++11 standard utility class.
// https://en.cppreference.com/w/cpp/types/integral_constant.html
#endif
template <class T, T constValue>
struct integral_constant {
	static constexpr T value = constValue;

	using value_type = T;
	using type		 = integral_constant;

				  constexpr	operator value_type()   const noexcept { return value; } // implicit conversion to value_type
	AXE_NODISCARD constexpr value_type operator()() const noexcept { return value; }
};

template< bool B > using bool_constant = integral_constant<bool, B>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;


//+----------------------------------------------+
//|       Relationships and property queries     |
//+----------------------------------------------+
#if 0
#pragma mark------------ is_same --------------
// is_same: https://en.cppreference.com/w/cpp/types/is_same.html
#endif
template <class T1, class T2>
struct is_same : public false_type {};

template<class T>
struct is_same<T, T> : public true_type {};

template <class T1, class T2> inline constexpr bool is_same_v = is_same<T1, T2>::value;


#if 0
#pragma mark------------ is_base_of --------------
// is_base_of: https://en.cppreference.com/w/cpp/types/is_base_of.html
// __is_base_of is compiler intrinsic (a built-in feature provided by the compiler)
#endif
template <class Base, class Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template <class Base, class Derived> inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;


#if 0
#pragma mark------------ extent --------------
// extent: https://en.cppreference.com/w/cpp/types/extent.html
#endif
template <class T, unsigned N = 0>
struct extent_help : integral_constant<size_t, 0> {};
 
template <class T>
struct extent_help<T[], 0> : integral_constant<size_t, 0> {};

template <class T, unsigned N>
struct extent_help<T[], N> : extent_help<T, N - 1> {};

template <class T, unsigned I>
struct extent_help<T[I], 0> : integral_constant<size_t, I> {};

template <class T, unsigned I, unsigned N>
struct extent_help<T[I], N> : extent_help<T, N - 1> {};

template <typename T, unsigned N = 0> // extent uses unsigned instead of size_t.
struct extent : extent_help<T, N> {};

template <class T, unsigned N = 0> inline constexpr auto extent_v = extent<T, N>::value;


//+----------------------------------------------+
//|             Type transformations             |
//+----------------------------------------------+
#if 0
#pragma mark------------ underlying_type --------------
// underlying_type: https://en.cppreference.com/w/cpp/types/underlying_type.html
// __underlying_type is compiler intrinsic (a built-in feature provided by the compiler)
#endif
template <class T>
struct underlying_type{ using type = typename __underlying_type(T); };

template <class T> using underlying_type_t					= typename underlying_type<T>::type;
template <class T> using underlying_type_t_pointer			= typename underlying_type_t<T>*;
template <class T> using underlying_type_t_reference		= typename underlying_type_t<T>&;
template <class T> using underlying_type_t_const_pointer	= const typename underlying_type_t_pointer<T>;
template <class T> using underlying_type_t_const_reference	= const typename underlying_type_t_reference<T>;


#if 0
#pragma mark------------ enable_if --------------
// enable_if: https://en.cppreference.com/w/cpp/types/enable_if.html
#endif
template <bool B, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> { using type = T; };

template <bool B, class T = void> using enable_if_t  = typename enable_if<B, T>::type;


#if 0
#pragma mark------------ disable_if --------------
#endif
template <bool B, class T = void>
struct disable_if {};

template <class T>
struct disable_if<false, T> { using type = T; };

template <bool B, class T = void> using disable_if_t = typename disable_if<B, T>::type;


#if 0
#pragma mark------------ conditional --------------
// conditional: https://en.cppreference.com/w/cpp/types/conditional.html
#endif
template <bool B, class T, class F>
struct conditional { using type = T; };
 
template<class T, class F>
struct conditional<false, T, F> { using type = F; };

template <bool B, class T, class F> using conditional_t = typename conditional<B, T, F>::type;


//+----------------------------------------------+
//|               Metafunctions                  |
//+----------------------------------------------+
#if 0
#pragma mark------------ conjunction --------------
// conjunction: This is a C++17 standard utility class.
// http://en.cppreference.com/w/cpp/types/conjunction
#endif
template <class...>
struct conjunction : true_type {};

template <class B1>
struct conjunction<B1> : B1 {};

template <class B1, class... Bn>
struct conjunction<B1, Bn...> : conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

template <typename... Bn> inline constexpr bool conjunction_v = conjunction<Bn...>::value;


#if 0
#pragma mark------------ disjunction --------------
// disjunction: This is a C++17 standard utility class.
// http://en.cppreference.com/w/cpp/types/disjunction
#endif
template <class...>
struct disjunction : false_type {};
 
template <class B1>
struct disjunction<B1> : B1 {};
 
template <class B1, class... Bn>
struct disjunction<B1, Bn...> : conditional_t<bool(B1::value), B1, disjunction<Bn...>> {};

template <typename... Bn> inline constexpr bool disjunction_v = disjunction<Bn...>::value;


//+----------------------------------------------+
//|              Type modifications              |
//+----------------------------------------------+
#if 0
#pragma mark------------ remove_const --------------
// remove_const: https://en.cppreference.com/w/cpp/types/remove_cv.html
#endif
template <class T> struct remove_const						   { using type = T; };
template <class T> struct remove_const<const T>				   { using type = T; };
template <class T> struct remove_const<const T[]>			   { using type = T[]; };  // typedef T type[];
template <class T, size_t N> struct remove_const<const T[N]>   { using type = T[N]; }; // typedef T type[N];

template <class T> using remove_const_t = typename remove_const<T>::type;


#if 0
#pragma mark------------ remove_volatile --------------
// remove_volatile: https://en.cppreference.com/w/cpp/types/remove_cv.html
#endif
template <class T> struct remove_volatile						   { using type = T; };
template <class T> struct remove_volatile<volatile T>			   { using type = T; };
template <class T> struct remove_volatile<volatile T[]>			   { using type = T[]; };
template <class T, size_t N> struct remove_volatile<volatile T[N]> { using type = T[N]; };

template <class T> using remove_volatile_t = typename remove_volatile<T>::type;


#if 0
#pragma mark------------ remove_pointer --------------
// remove_pointer: https://en.cppreference.com/w/cpp/types/remove_pointer.html
#endif
template <class T> struct remove_pointer						{ using type = T; };
template <class T> struct remove_pointer<T*>					{ using type = T; };
template <class T> struct remove_pointer<T* const>				{ using type = T; };
template <class T> struct remove_pointer<T* volatile>			{ using type = T; };
template <class T> struct remove_pointer<T* const volatile>		{ using type = T; };

template <class T> using remove_pointer_t = typename remove_pointer<T>::type;


#if 0
#pragma mark------------ remove_reference --------------
// remove_reference: https://en.cppreference.com/w/cpp/types/remove_reference.html
#endif
template <class T> struct remove_reference					{ using type = T; };
template <class T> struct remove_reference<T&>				{ using type = T; };
template <class T> struct remove_reference<T&&>				{ using type = T; };

template <class T> using remove_reference_t					= typename remove_reference<T>::type;
template <class T> using remove_reference_t_pointer			= typename remove_reference_t<T>*;
template <class T> using remove_reference_t_reference		= typename remove_reference_t<T>&;
template <class T> using remove_reference_t_const_pointer	= const typename remove_reference_t_pointer<T>;
template <class T> using remove_reference_t_const_reference	= const typename remove_reference_t_reference<T>;


#if 0
#pragma mark------------ remove_cv --------------
//remove_cv: https://en.cppreference.com/w/cpp/types/remove_cv.html
#endif
template <class T>
struct remove_cv {
	using type = typename remove_volatile_t< typename remove_const_t<T> >;
};

template <typename T> using remove_cv_t = typename remove_cv<T>::type;


#if 0
#pragma mark------------ remove_cref --------------
#endif
template <class T>
struct remove_cref {
	using type = typename remove_const_t< typename remove_reference_t<T> >;
};

template <typename T> using remove_cref_t = typename remove_cref<T>::type;


#if 0
#pragma mark------------ remove_cvref --------------
#endif
template <class T>
struct remove_cvref {
	using type = typename remove_volatile_t< typename remove_const_t< typename remove_reference_t<T> > >;
};

template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;


#if 0
#pragma mark------------ remove_extent --------------
// remove_extent: https://en.cppreference.com/w/cpp/types/remove_extent.html
#endif
template <class T> struct remove_extent					{ using type = T; };
template <class T> struct remove_extent<T[]>			{ using type = T; };
template <class T, size_t N> struct remove_extent<T[N]>	{ using type = T; };

template <class T> using remove_extent_t					= typename remove_extent<T>::type;
template <class T> using remove_extent_t_pointer			= typename remove_extent_t<T>*;
template <class T> using remove_extent_t_reference			= typename remove_extent_t<T>&;
template <class T> using remove_extent_t_const_pointer		= const typename remove_extent_t_pointer<T>;
template <class T> using remove_extent_t_const_reference	= const typename remove_extent_t_reference<T>;

#if 0
#pragma mark------------ remove_all_extents --------------
// remove_all_extents: https://en.cppreference.com/w/cpp/types/remove_all_extents.html
#endif
template <class T> struct remove_all_extents					{ using type = T; };
template <class T> struct remove_all_extents<T[]>				{ using type = typename remove_all_extents<T>::type; };
template <class T, size_t N> struct remove_all_extents<T[N]>	{ using type = typename remove_all_extents<T>::type; };

template <class T> using remove_all_extents_t = typename remove_all_extents<T>::type;


#if 0
#pragma mark------------ add_pointer --------------
// add_pointer: https://en.cppreference.com/w/cpp/types/add_pointer.html
#endif
namespace _internal
{
    template<class T>
    struct type_identity { using type = T; }; // or use std::type_identity (since C++20)
 
    template<class T>
    auto try_add_pointer(int) -> type_identity<typename remove_reference_t_pointer<T>>; // usual case
 
    template<class T>
    auto try_add_pointer(...) -> type_identity<T>; // unusual case (cannot form std::remove_reference<T>::type*)
} // namespace _internal
 
template<class T>
struct add_pointer : decltype(_internal::try_add_pointer<T>(0)) {};

template <class T> using add_pointer_t = typename add_pointer<T>::type;


//+----------------------------------------------+
//|                Type categories               |
//+----------------------------------------------+
#if 0
#pragma mark------------ is_reference --------------
// is_reference: https://en.cppreference.com/w/cpp/types/is_reference.html
#endif
template <class T> struct is_reference		: false_type {};
template <class T> struct is_reference<T&>	: true_type  {};
template <class T> struct is_reference<T&&>	: true_type  {};

template <class T> inline constexpr bool is_reference_v = is_reference<T>::value;


#if 0
#pragma mark------------ is_const --------------
// is_const: https://en.cppreference.com/w/cpp/types/is_const.html
#endif
template <class T> struct is_const			: false_type {};
template <class T> struct is_const<const T>	: true_type  {};

template <class T> inline constexpr bool is_const_v = is_const<T>::value;


#if 0
#pragma mark------------ is_pointer --------------
// is_pointer: https://en.cppreference.com/w/cpp/types/is_pointer.html
#endif
template <class T> struct is_pointer					: false_type {};
template <class T> struct is_pointer<T*>				: true_type  {};
template <class T> struct is_pointer<T* const>			: true_type  {};
template <class T> struct is_pointer<T* volatile>		: true_type  {};
template <class T> struct is_pointer<T* const volatile>	: true_type  {};

template <class T> inline constexpr bool is_pointer_v = is_pointer<T>::value;


#if 0
#pragma mark------------ is_void --------------
// is_void: https://en.cppreference.com/w/cpp/types/is_void.html
#endif
template <class T> inline constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;
template <class T> struct is_void : bool_constant<is_void_v<T>> {};


#if 0
#pragma mark------------ is_function --------------
// is_function: https://en.cppreference.com/w/cpp/types/is_function.html
// Walter E. Brown CppCon 2019: https://www.youtube.com/watch?v=OAmWHmwlMwI
//   "only function types and reference types can't be const qualified."
#endif
template<class T>
struct is_function : bool_constant<!is_const_v<const T> && !is_reference_v<T>> {};

template <class T> inline constexpr bool is_function_v = is_function<T>::value;


#if 0
#pragma mark------------ is_enum --------------
// is_enum: https://en.cppreference.com/w/cpp/types/is_enum.html
// __is_enum: is a compiler intrinsic (a built-in feature provided by the compiler)
#endif

template <class T> struct is_enum : bool_constant<__is_enum(T)> {};

template <class T> inline constexpr bool is_enum_v = is_enum<T>::value;


#if 0
#pragma mark------------ is_array --------------
// is_array: https://en.cppreference.com/w/cpp/types/is_array.html
#endif
template <class T> struct is_array					: false_type {};
template <class T> struct is_array<T[]>				: true_type  {};
template <class T, size_t N> struct is_array<T[N]>	: true_type  {};

template <class T> inline constexpr bool is_array_v = is_array<T>::value;


#if 0
#pragma mark------------ is_integral --------------
// is_integral: https://en.cppreference.com/w/cpp/types/is_integral.html
#endif
template <class T> struct is_integral_helper  : false_type {};

#define AXE_MACRO_OP(T) \
	template <> struct is_integral_helper<T> : true_type {}; \
//----
	AXE_TYPE_LIST_BOOL(AXE_MACRO_OP)
	AXE_TYPE_LIST_SINT(AXE_MACRO_OP)
	AXE_TYPE_LIST_UINT(AXE_MACRO_OP)
	AXE_TYPE_LIST_CHAR(AXE_MACRO_OP)
#undef AXE_MACRO_OP

template <class T>
struct is_integral : public is_integral_helper<typename remove_cv_t<T>> {};

template <class T> inline constexpr bool is_integral_v = is_integral<T>::value;


#if 0
#pragma mark------------ is_floating_point --------------
// is_floating_point: https://en.cppreference.com/w/cpp/types/is_floating_point.html
#endif
template <class T> struct is_floating_point_helper  : public false_type {};

#define AXE_MACRO_OP(T) \
	template <> struct is_floating_point_helper<T> : public true_type {}; \
//----
	AXE_TYPE_LIST_FLOAT(AXE_MACRO_OP)
#undef AXE_MACRO_OP

template <class T>
struct is_floating_point : public is_floating_point_helper<typename remove_cv_t<T>> {};

template <class T> inline constexpr bool is_floating_point_v = is_floating_point<T>::value;


#if 0
#pragma mark------------ is_arithmetic --------------
// is_arithmetic: https://en.cppreference.com/w/cpp/types/is_arithmetic.html
#endif
template<class T>
struct is_arithmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T>> {};

template <class T> inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;


//+----------------------------------------------+
//|                Type properties               |
//+----------------------------------------------+
#if 0
#pragma mark------------ is_signed --------------
// is_signed: https://cppreference.com/w/cpp/types/is_signed.html
#endif
template <class T, bool = is_arithmetic_v<T>>
struct is_signed_helper  : public bool_constant<T(-1) < T(0)> {};

template<class T> struct
is_signed_helper<T, false> : false_type {};

template<class T>
struct is_signed : is_signed_helper<T>::type {};

template <class T> inline constexpr bool is_signed_v = is_signed<T>::value;


#if 0
#pragma mark------------ is_unsigned --------------
// is_unsigned: https://en.cppreference.com/w/cpp/types/is_unsigned.html
#endif
template <class T, bool = is_arithmetic_v<T>>
struct is_unsigned_helper : bool_constant<T(0) < T(-1)> {};

template <class T>
struct is_unsigned_helper<T, false> : false_type {};

template<class T>
struct is_unsigned : is_unsigned_helper<T>::type {};

template <class T> inline constexpr bool is_unsigned_v = is_unsigned<T>::value;


#if 0
#pragma mark------------ decay --------------
// decay: https://en.cppreference.com/w/cpp/types/decay.html
#endif
template<typename T>
struct decay {
private:
	using U = typename remove_reference_t<T>;
public:
	using type = typename conditional< 
		is_array_v<U>,
		typename remove_extent_t_pointer<U>,
		typename conditional<
			is_function_v<U>,
			typename add_pointer_t<U>,
			typename remove_cv_t<U>
		>::type
	>::type;
};

template <typename T> using decay_t = typename decay<T>::type;


//----
template<size_t N> struct typeBySize;

template<>
struct typeBySize<1> {
	using Int  = i8;
	using UInt = u8;
	using Char = char;
};

template<>
struct typeBySize<2> {
	using Int  = i16;
	using UInt = u16;
	using Char = char16_t;
};

template<>
struct typeBySize<4> {
	using Int   = i32;
	using UInt  = u32;
	using Float = f32;
	using Char  = char32_t;
};

template<>
struct typeBySize<8> {
	using Int   = i64;
	using UInt  = u64;
	using Float = f64;
};

template<>
struct typeBySize<16> {
	using Float = f128;
};

} // namespace axe