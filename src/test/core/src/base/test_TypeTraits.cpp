#include <axe_core/base/UnitTest.h>

namespace axe {

#if 0
#pragma mark------------ is_same --------------
#endif
struct Test_TypeTraits__is_same : public UnitTestBase {
	using This = Test_TypeTraits__is_same;

	int m;

	template <class Obj, class Member>
	static intptr_t s_foo(Member Obj::*p) { return intptr_t(0); }

	using FuncReturnType1 = decltype(s_foo<This, int>(&This::m));
	using FuncReturnType2 = decltype(s_foo(&This::m));

	AXE_STATIC_ASSERT(is_same_v<FuncReturnType1, FuncReturnType2>);

	AXE_STATIC_ASSERT((is_same<char, char>::value));
	AXE_STATIC_ASSERT(!(is_same_v<char AXE_COMMA unsigned char>));
	AXE_STATIC_ASSERT(!(is_same_v<char AXE_COMMA signed char>));

	AXE_STATIC_ASSERT( is_same_v<int AXE_COMMA ::std::int32_t>);
	AXE_STATIC_ASSERT( is_same_v<int AXE_COMMA signed int>);
	AXE_STATIC_ASSERT(!(is_same_v<int AXE_COMMA unsigned int>));

	AXE_STATIC_ASSERT( is_same_v<int AXE_COMMA decltype(12345)>);
	AXE_STATIC_ASSERT(!(is_same_v<int AXE_COMMA const int>));

	AXE_STATIC_ASSERT(is_same_v<const int * AXE_COMMA int const *>);
	AXE_STATIC_ASSERT(!is_same_v<const int * AXE_COMMA int* const>);

	AXE_STATIC_ASSERT(!(is_same_v<byte AXE_COMMA ::std::byte>));
}; // Test_TypeTraits__is_same


#if 0
#pragma mark------------ is_base_of --------------
#endif
struct Test_TypeTraits__is_base_of : public UnitTestBase
{
	class A {};
	class B : A {};
	class C : B {};
	class D {};
	union E {};
	using I = int;
 
	AXE_STATIC_ASSERT((is_base_of_v<A, A>));
	AXE_STATIC_ASSERT((is_base_of_v<A, B>));
	AXE_STATIC_ASSERT((is_base_of_v<A, C>));

	AXE_STATIC_ASSERT(!(is_base_of_v<A, D>));
	AXE_STATIC_ASSERT(!(is_base_of_v<B, A>));
	AXE_STATIC_ASSERT(!(is_base_of_v<E, E>));
	AXE_STATIC_ASSERT(!(is_base_of_v<I, I>));
}; // Test_TypeTraits__is_base_of


#if 0
#pragma mark------------ extent --------------
#endif
struct Test_TypeTraits__extent : public UnitTestBase {
	int a[4];

	AXE_STATIC_ASSERT(extent_v<int[3]> == 3);
	AXE_STATIC_ASSERT(extent_v<int[3], 0> == 3);
	AXE_STATIC_ASSERT(extent_v<int[3][4], 0> == 3);
	AXE_STATIC_ASSERT(extent_v<int[3][4], 1> == 4);
	AXE_STATIC_ASSERT(extent_v<int[3][4], 2> == 0);
	AXE_STATIC_ASSERT(extent_v<int[]> == 0);

	void test() {
		const auto ext = extent<int['*']>{};
		AXE_STATIC_ASSERT('*' == 42);
		AXE_TEST_CHECK(ext == 42); // with implicit conversion to std::size_t

		const int ints[]{ 1, 2, 3, 4 };
		AXE_TEST_CHECK(extent_v<decltype(ints)> == 4); // array size

		[[maybe_unused]] int ary[][3] = { { 1, 2, 3 } };

		// ary[0] is of type reference to 'int[3]', so, the extent
		// cannot be calculated correctly and it returns 0
		AXE_TEST_CHECK(is_same_v<decltype(ary[0]) AXE_COMMA int(&)[3]>);
		AXE_TEST_CHECK(extent_v<decltype(ary[0])> == 0);

		// removing reference gives correct extent value 3
		AXE_TEST_CHECK(extent_v<remove_cvref_t<decltype(ary[0])>> == 3);
	}
}; // Test_TypeTraits__extent


#if 0
#pragma mark------------ integral_constant --------------
#endif
struct Test_TypeTraits__integral_constant : public UnitTestBase {
	void test1()
	{
		using two_t	 = integral_constant<int, 2>;
		using four_t = integral_constant<int, 4>;

		AXE_TEST_CHECK(!(is_same_v<two_t, four_t>));
		AXE_TEST_CHECK(two_t::value * 2 == four_t::value);
		AXE_TEST_CHECK(two_t() << 1 == four_t() >> 0);
	}

	void test2()
	{
		enum class E
		{
			e1,
			e2
		};

		using c1 = integral_constant<E, E::e1>;
		using c2 = integral_constant<E, E::e2>;
		AXE_TEST_CHECK(c1::value != E::e2);
		AXE_TEST_CHECK(c1() == E::e1);
		AXE_TEST_CHECK(is_same_v<c2 AXE_COMMA c2>);
	}
}; // Test_TypeTraits__integral_constant


struct Test_TypeTraits__conditional : public UnitTestBase {
	void test() {
		using Type1 = conditional_t<true,  int, double>;
		using Type2 = conditional_t<false, int, double>;
		using Type3 = conditional_t<sizeof(int) >= sizeof(double), int, double>;

		AXE_TEST_CHECK(is_same_v<Type1 AXE_COMMA int>);
		AXE_TEST_CHECK(is_same_v<Type2 AXE_COMMA double>);
		AXE_TEST_CHECK(is_same_v<Type3 AXE_COMMA double>);
	}
}; // Test_TypeTraits__conditional


#if 0
#pragma mark------------ decay --------------
#endif
template <typename T, typename Answer> constexpr bool is_decay_equ = is_same_v<decay_t<T>, Answer>;

struct Test_TypeTraits__decay : public UnitTestBase {
	using This = Test_TypeTraits__decay;

	AXE_STATIC_ASSERT(is_decay_equ<int, int>);
	AXE_STATIC_ASSERT( is_decay_equ<int, int>);
	AXE_STATIC_ASSERT(!is_decay_equ<int, float>);

	AXE_STATIC_ASSERT( is_decay_equ<int&, int>);
	AXE_STATIC_ASSERT( is_decay_equ<int&&, int>);
	AXE_STATIC_ASSERT( is_decay_equ<const int&, int>);

	AXE_STATIC_ASSERT( is_decay_equ<int[2], int*>);
	AXE_STATIC_ASSERT(!is_decay_equ<int[4][2], int*>);
	AXE_STATIC_ASSERT(!is_decay_equ<int[4][2], int**>);
	AXE_STATIC_ASSERT( is_decay_equ<int[4][2], int (*)[2]>);

	static int s_foo(float x) { return int(x); }

	AXE_STATIC_ASSERT( is_decay_equ<int(int), int(*)(int)> );

	void test() {
		int (*ptr)(float) = &This::s_foo;

		AXE_TEST_CHECK(! is_decay_equ<decltype(ptr) AXE_COMMA decltype(This::s_foo)>);
		AXE_TEST_CHECK(  is_decay_equ<decltype(This::s_foo) AXE_COMMA decltype(ptr)>);
	}

}; // Test_TypeTraits__decay

#if 0
#pragma mark------------ underlying_type --------------
#endif
struct Test_TypeTraits__underlying_type : public UnitTestBase {

	enum e1 {};
	enum class e2 {};
	enum class e3 : unsigned {};
	enum class e4 : int {};
	enum class e5 : uint8_t {};

	AXE_STATIC_ASSERT(is_same_v<underlying_type_t<e1> AXE_COMMA int>);
	AXE_STATIC_ASSERT(is_same_v<underlying_type_t<e2> AXE_COMMA int>);
	AXE_STATIC_ASSERT(!(is_same_v<underlying_type_t<e3> AXE_COMMA int>));
	AXE_STATIC_ASSERT(is_same_v<underlying_type_t<e4> AXE_COMMA int>);
	AXE_STATIC_ASSERT(is_same_v<underlying_type_t<e5> AXE_COMMA uint8_t>);
}; // Test_TypeTraits__underlying_type


#if 0
#pragma mark------------ enable_if --------------
#endif
struct Test_TypeTraits__enable_if : public UnitTestBase {
	// primary template
	template <class T, class ENABLE = void>
	struct A {
		static constexpr int kCLassId = 1;
	};

	// specialization for floating point types
	template <class T>
	struct A <T, typename enable_if_t < is_floating_point_v<T> > > {
		static constexpr int kCLassId = 2;
	};

	void test()
	{
		AXE_TEST_CHECK(A<int>::kCLassId == 1);
		AXE_TEST_CHECK(A<double>::kCLassId == 2);
	}
};


#if 0
#pragma mark------------ disable_if --------------
#endif
struct Test_TypeTraits__disable_if : public UnitTestBase {
	// primary template
	template <class T, class ENABLE = void>
	struct A {
		static constexpr int kCLassId = 1;
	};

	// specialization for non-floating point types
	template <class T>
	struct A <T, typename disable_if_t < is_floating_point_v<T> > > {
		static constexpr int kCLassId = 2;
	};

	void test()
	{
		AXE_TEST_CHECK(A<int>::kCLassId == 2);
		AXE_TEST_CHECK(A<double>::kCLassId == 1);
	}
}; // Test_TypeTraits__disable_if


#if 0
#pragma mark------------ is_reference --------------
#endif
struct Test_TypeTraits__is_reference : public UnitTestBase {
	class A {};

	AXE_STATIC_ASSERT(!is_reference_v<A>);
	AXE_STATIC_ASSERT( is_reference_v<A&>);
	AXE_STATIC_ASSERT( is_reference_v<A&&>);
	AXE_STATIC_ASSERT(!is_reference_v<long>);
	AXE_STATIC_ASSERT( is_reference_v<long&>);
	AXE_STATIC_ASSERT( is_reference_v<long&&>);
	AXE_STATIC_ASSERT(!is_reference_v<double*>);
	AXE_STATIC_ASSERT( is_reference_v<double*&>);
	AXE_STATIC_ASSERT( is_reference_v<double*&&>);

}; // Test_TypeTraits__is_reference


#if 0
#pragma mark------------ is_const --------------
#endif
struct Test_TypeTraits__is_const : public UnitTestBase {

	AXE_STATIC_ASSERT(!is_const_v<int>);
	AXE_STATIC_ASSERT( is_const_v<const int>);
	AXE_STATIC_ASSERT(!is_const_v<int*>);

	AXE_STATIC_ASSERT(!is_const_v<const int&>);
	AXE_STATIC_ASSERT( is_const_v<remove_reference_t<const int&>>);

	AXE_STATIC_ASSERT_MSG( is_const_v<int* const>, "Because the pointer itself can't be changed but the int pointed at can.");
	AXE_STATIC_ASSERT_MSG(!is_const_v<const int*>, "Because the pointer itself can be changed but not the int pointed at.");

	struct S
	{
		void foo() const {}
		void bar() const {}
	};

	void test() {
		// A const member function is const in a different way:
		AXE_STATIC_ASSERT_MSG(!is_const_v<decltype(&S::foo)>,
			"Because &S::foo is a pointer. only function types and reference types can't be const qualified");

		using S_mem_fun_ptr = void (S::*)() const;

		S_mem_fun_ptr sfp = &S::foo;
		sfp				  = &S::bar; // OK, can be re-pointed
		AXE_STATIC_ASSERT_MSG(!is_const_v<decltype(sfp)>,
			"Because sfp is the same pointer type and thus can be re-pointed.");

		const S_mem_fun_ptr csfp = &S::foo;
		// csfp = &S::bar; // Would Compile Error, can not modified csfp cuz it is const value
		AXE_STATIC_ASSERT_MSG(is_const_v<decltype(csfp)>,
			"Because csfp cannot be re-pointed.");
	}
}; // Test_TypeTraits__is_const


#if 0
#pragma mark------------ is_function --------------
#endif
int g_foo() { return 0; }
void static s_foo() {}
AXE_STATIC_ASSERT( is_function_v<decltype(g_foo)>);
AXE_STATIC_ASSERT( is_function_v<decltype(s_foo)>);

struct Test_TypeTraits__is_function : public UnitTestBase {
	using This = Test_TypeTraits__is_function;

	int f() {}
	AXE_STATIC_ASSERT(!is_function_v<decltype(&This::f)>);
//	AXE_STATIC_ASSERT( is_function_v<decltype(This::f)>); // invalid syntax, cuz member functions cannot be referenced without &

	static int s(float) { return 0; }
	using StaticFuncPtr  = decltype(&This::s); //!!<--- int(*)(float) is a function pointer
	using StaticFuncType = decltype( This::s); //!!<--- int   (float) is a function type

	AXE_STATIC_ASSERT(!is_function_v<StaticFuncPtr>);
	AXE_STATIC_ASSERT( is_function_v<StaticFuncType>);

	using Func    = int(void);
	using FuncPtr = int(*)(void);
	AXE_STATIC_ASSERT( is_function_v<Func>);
	AXE_STATIC_ASSERT(!is_function_v<FuncPtr>);

	AXE_STATIC_ASSERT( is_function_v< int(int) >);
	AXE_STATIC_ASSERT(!is_function_v<int>);
#if AXE_CPLUSPLUS_20
	AXE_STATIC_ASSERT(!is_function_v<decltype([]{})>);
#endif

	using FuncObj = AXE_FUNC<void(int)>;
	AXE_STATIC_ASSERT(!is_function_v<FuncObj>);
	AXE_STATIC_ASSERT(!is_function_v<::std::function<void()>>);

	struct A
	{
		static double s_foo() { return 0; }
		int foo() const& { return 0; }
		void operator()() {}
	};
	AXE_STATIC_ASSERT( is_function_v<decltype( A::s_foo)>);
	AXE_STATIC_ASSERT(!is_function_v<decltype(&A::operator())> );

	template<typename> struct MyClassT {};
	template<class Obj, class Member>
	struct MyClassT<Member Obj::*> {
		using obj_type	  = Obj;
		using member_type = Member;
	};

	using ObjType    = MyClassT<decltype(&A::operator())>::obj_type;
	AXE_STATIC_ASSERT(is_same_v<ObjType, A>);

	using MemberType = MyClassT<decltype(&A::foo)>::member_type;	 // int() const& is function type
	AXE_STATIC_ASSERT(is_function_v<MemberType>);

	template <class Obj, class Member>
	static void s_template_func(Member Obj::* ptr) {}
	AXE_STATIC_ASSERT(is_function_v< decltype(s_template_func<This, int> )>);

}; // Test_TypeTraits__is_function


#if 0
#pragma mark------------ is_pointer --------------
#endif
struct Test_TypeTraits__is_pointer : public UnitTestBase {
	struct A
    {
        int m;
        void f() {}
    };
 
    int A::* mem_data_ptr = &A::m;     // a pointer to member data
    void (A::* mem_fun_ptr)() = &A::f; // a pointer to member function

	AXE_STATIC_ASSERT(! is_pointer<A>::value);
    AXE_STATIC_ASSERT(! is_pointer_v<A>);
    AXE_STATIC_ASSERT(! is_pointer<A>());
    AXE_STATIC_ASSERT(! is_pointer<A>{});
	AXE_STATIC_ASSERT(! is_pointer<A>()());
    AXE_STATIC_ASSERT(! is_pointer<A>{}());
    AXE_STATIC_ASSERT(  is_pointer_v<A*>);
    AXE_STATIC_ASSERT(  is_pointer_v<A const* volatile>);
    AXE_STATIC_ASSERT(! is_pointer_v<A&>);
    AXE_STATIC_ASSERT(! is_pointer_v<decltype(mem_data_ptr)>);
    AXE_STATIC_ASSERT(! is_pointer_v<decltype(mem_fun_ptr)>);
    AXE_STATIC_ASSERT(  is_pointer_v<void*>);
    AXE_STATIC_ASSERT(! is_pointer_v<int>);
    AXE_STATIC_ASSERT(  is_pointer_v<int*>);
    AXE_STATIC_ASSERT(  is_pointer_v<int**>);
    AXE_STATIC_ASSERT(! is_pointer_v<int[10]>);
    AXE_STATIC_ASSERT(! is_pointer_v<::std::nullptr_t>);
	AXE_STATIC_ASSERT(  is_pointer_v<void (*)()>);
};


#if 0
#pragma mark------------ is_void --------------
#endif
struct Test_TypeTraits__is_void : public UnitTestBase {
	using This = Test_TypeTraits__is_void;

	void foo() {}

	void test_Void() {
		AXE_TEST_CHECK(is_void_v<void> == true);
		AXE_TEST_CHECK(is_void_v<const void> == true);
		AXE_TEST_CHECK(is_void_v<volatile void> == true);
		AXE_TEST_CHECK(is_void_v<const volatile void> == true);
		AXE_TEST_CHECK(is_void_v<::std::void_t<>> == true);
		AXE_TEST_CHECK(is_void_v<void*> == false);
		AXE_TEST_CHECK(is_void_v< is_void<void> > == false);
	}

	void test_Int() {
		AXE_TEST_CHECK(is_void_v<int> == false);
	}

	void test_Func() {
		AXE_TEST_CHECK(is_void_v<decltype(&This::foo)> == false);
	}

	void test_Misc() {
		AXE_TEST_CHECK(is_void_v<decltype(this)> == false);
	}
}; // Test_TypeTraits__is_void


#if 0
#pragma mark------------ is_enum --------------
#endif
struct Test_TypeTraits__is_enum : public UnitTestBase {
	enum E {};
	struct A { enum E {}; };
	enum Ec : int {};

	AXE_STATIC_ASSERT( is_enum_v<E>);
	AXE_STATIC_ASSERT(!is_enum_v<A>);
	AXE_STATIC_ASSERT( is_enum_v<A::E>);
	AXE_STATIC_ASSERT( is_enum_v<Ec>);
	AXE_STATIC_ASSERT(!is_enum_v<int>);

}; // Test_TypeTraits__is_enum


#if 0
#pragma mark------------ is_array --------------
#endif
struct Test_TypeTraits__is_array : public UnitTestBase {
	using This = Test_TypeTraits__is_array;

	AXE_STATIC_ASSERT(is_array_v<This[3]>);

	class A {};
	AXE_STATIC_ASSERT(!is_array_v<A>);
	AXE_STATIC_ASSERT( is_array_v<A[]>);
	AXE_STATIC_ASSERT( is_array_v<A[3]>);

	AXE_STATIC_ASSERT(!is_array_v<float>);
	AXE_STATIC_ASSERT(!is_array_v<int>);
	AXE_STATIC_ASSERT( is_array_v<int[]>);
	AXE_STATIC_ASSERT( is_array_v<int[3]>);
	AXE_STATIC_ASSERT(!is_array_v<::std::array<int, 3>>);

}; // Test_TypeTraits__is_enum

#if 0
#pragma mark------------ is_integral --------------
#endif
struct Test_TypeTraits__is_integral : public UnitTestBase {
	void test_Int() {
		AXE_TEST_CHECK(is_integral_v<int*> == false);
		AXE_TEST_CHECK(is_integral_v<int> == true);
		AXE_TEST_CHECK(is_integral_v<const int> == true);
		AXE_TEST_CHECK(!is_integral_v<::std::atomic_int>);
	}

	void test_Float() {
		AXE_TEST_CHECK(is_integral_v<float> == false);
		AXE_TEST_CHECK(is_integral_v<double> == false);
		AXE_TEST_CHECK(is_integral_v<long double> == false);
	}

	bool _bitBool : 1;
	void test_Bool() {
		AXE_TEST_CHECK(is_integral_v<bool> == true);
		AXE_TEST_CHECK(is_integral_v<const bool> == true);
		AXE_TEST_CHECK(is_integral_v<decltype(_bitBool)>);
	}

	void test_Char() {
		AXE_TEST_CHECK(is_integral_v<char> == true);
		AXE_TEST_CHECK(is_integral_v<unsigned char> == true);
		AXE_TEST_CHECK(is_integral_v<signed char> == true);
		AXE_TEST_CHECK(is_integral_v<wchar_t>);
		AXE_TEST_CHECK(is_integral_v<char16_t>);
		AXE_TEST_CHECK(is_integral_v<char32_t>);
	}

	void test_Misc() {
		AXE_TEST_CHECK(is_integral_v<void> == false);
		AXE_TEST_CHECK(!is_integral_v<::std::nullptr_t>);
		AXE_TEST_CHECK(is_integral_v<decltype(this)> == false);
	}
};


#if 0
#pragma mark------------ is_floating_point --------------
#endif
struct Test_TypeTraits__is_floating_point : public UnitTestBase {
	void test_Int() {
		AXE_TEST_CHECK(is_floating_point_v<int*> == false);
		AXE_TEST_CHECK(is_floating_point_v<int&> == false);
		AXE_TEST_CHECK(is_floating_point_v<int> == false);
		AXE_TEST_CHECK(is_floating_point_v<const int> == false);
		AXE_TEST_CHECK(is_floating_point_v<const int&> == false);
	}

	void test_Float() {
		AXE_TEST_CHECK(is_floating_point_v<float>);
		AXE_TEST_CHECK(is_floating_point_v<double>);
		AXE_TEST_CHECK(is_floating_point_v<long double>);

		AXE_TEST_CHECK(!(is_floating_point_v<float&>));
		AXE_TEST_CHECK(!(is_floating_point_v<double&>));
	}

	bool _bitBool : 1;
	void test_Bool() {
		AXE_TEST_CHECK(!(is_floating_point_v<bool>));
		AXE_TEST_CHECK(!(is_floating_point_v<const bool>));
		AXE_TEST_CHECK(!is_floating_point_v<decltype(_bitBool)>);
	}

	void test_Char() {
		AXE_TEST_CHECK(!is_floating_point_v<char>);
		AXE_TEST_CHECK(!is_floating_point_v<unsigned char>);
		AXE_TEST_CHECK(!is_floating_point_v<signed char>);
		AXE_TEST_CHECK(!is_floating_point_v<wchar_t>);
		AXE_TEST_CHECK(!is_floating_point_v<char16_t>);
		AXE_TEST_CHECK(!is_floating_point_v<char32_t>);
	}

	void test_Misc() {
		AXE_TEST_CHECK(!is_floating_point_v<void>);
		AXE_TEST_CHECK(!is_floating_point_v<::std::nullptr_t>);
		AXE_TEST_CHECK(!is_floating_point_v<decltype(this)>);
	}
};


#if 0
#pragma mark------------ is_arithmetic --------------
#endif
struct Test_TypeTraits__is_arithmetic : public UnitTestBase {
	void test_Int() {
		AXE_TEST_CHECK(is_arithmetic_v<int>);
		AXE_TEST_CHECK(is_arithmetic_v<const int>);
		AXE_TEST_CHECK(is_arithmetic_v<size_t>);
		AXE_TEST_CHECK(!is_arithmetic_v<int*>);
		AXE_TEST_CHECK(!is_arithmetic_v<int&>);
		AXE_TEST_CHECK(!is_arithmetic_v<const int&>);
		AXE_TEST_CHECK(!is_arithmetic_v<::std::atomic_int>);

		AXE_TEST_CHECK(is_arithmetic_v<byte>);
		AXE_TEST_CHECK(!is_arithmetic_v<::std::byte>);
	}

	void test_Float() {
		AXE_TEST_CHECK(is_arithmetic_v<float>);
		AXE_TEST_CHECK(is_arithmetic_v<double>);
		AXE_TEST_CHECK(is_arithmetic_v<long double>);

		AXE_TEST_CHECK(!is_arithmetic_v<float*>);
		AXE_TEST_CHECK(!is_arithmetic_v<const double*>);
		AXE_TEST_CHECK(!(is_arithmetic_v<float&>));
		AXE_TEST_CHECK(!(is_arithmetic_v<long double&&>));
	}

	bool _bitBool : 1;
	void test_Bool() {
		AXE_TEST_CHECK(is_arithmetic_v<bool>);
		AXE_TEST_CHECK(is_arithmetic_v<const bool>);
		AXE_TEST_CHECK(!is_arithmetic_v<bool&>);
		AXE_TEST_CHECK(is_arithmetic_v<decltype(_bitBool)>);
		AXE_TEST_CHECK(!is_arithmetic_v<::std::atomic_bool>);
	}

	void test_Char() {
		AXE_TEST_CHECK(is_arithmetic_v<char>);
		AXE_TEST_CHECK(!is_arithmetic_v<char&>);
		AXE_TEST_CHECK(is_arithmetic_v<unsigned char>);
		AXE_TEST_CHECK(!is_arithmetic_v<unsigned char*>);
		AXE_TEST_CHECK(is_arithmetic_v<signed char>);
		AXE_TEST_CHECK(is_arithmetic_v<wchar_t>);
		AXE_TEST_CHECK(is_arithmetic_v<char16_t>);
		AXE_TEST_CHECK(is_arithmetic_v<char32_t>);
	}

	enum class B : int { e };
	void test_Enum() {
		AXE_TEST_CHECK(!is_arithmetic_v<B>);
		AXE_TEST_CHECK(!is_arithmetic_v<decltype(B::e)>);
	}

	void test_Misc() {
		AXE_TEST_CHECK(!is_arithmetic_v<void>);
		AXE_TEST_CHECK(!is_arithmetic_v<::std::nullptr_t>);
		AXE_TEST_CHECK(!is_arithmetic_v<decltype(this)>);
	}
};


#if 0
#pragma mark------------ _test_TypeTraits_Helper --------------
#endif
struct _test_TypeTraits_Helper : public StaticAbstructClass {

	#if 0
	#pragma mark------------ Core --------------
	#endif
	struct Core : public StaticAbstructClass
	{
		static void test_integral_constant() {
			AXE_TEST_CASE(Test_TypeTraits__integral_constant, test1());
			AXE_TEST_CASE(Test_TypeTraits__integral_constant, test2());
		}
	}; // Core

	#if 0
	#pragma mark------------ Relationships and property queries --------------
	#endif
	struct RelationshipsAndPropertyQueries : public StaticAbstructClass
	{
		static void test_extent()
		{
			AXE_TEST_CASE(Test_TypeTraits__extent, test());
		}
	}; // RelationshipsAndPropertyQueries

	#if 0
	#pragma mark------------ Type categories --------------
	#endif
	struct TypeCategories : public StaticAbstructClass
	{
		static void test_is_const() {
			AXE_TEST_CASE(Test_TypeTraits__is_const, test());
		}

		static void test_is_void() {
			AXE_TEST_CASE(Test_TypeTraits__is_void, test_Void());
			AXE_TEST_CASE(Test_TypeTraits__is_void, test_Int());
			AXE_TEST_CASE(Test_TypeTraits__is_void, test_Func());
			AXE_TEST_CASE(Test_TypeTraits__is_void, test_Misc());
		}

		static void test_is_integral() {
			AXE_TEST_CASE(Test_TypeTraits__is_integral, test_Int());
			AXE_TEST_CASE(Test_TypeTraits__is_integral, test_Float());
			AXE_TEST_CASE(Test_TypeTraits__is_integral, test_Bool());
			AXE_TEST_CASE(Test_TypeTraits__is_integral, test_Char());
			AXE_TEST_CASE(Test_TypeTraits__is_integral, test_Misc());
		}

		static void test_is_floating_point()
		{
			AXE_TEST_CASE(Test_TypeTraits__is_floating_point, test_Int());
			AXE_TEST_CASE(Test_TypeTraits__is_floating_point, test_Float());
			AXE_TEST_CASE(Test_TypeTraits__is_floating_point, test_Bool());
			AXE_TEST_CASE(Test_TypeTraits__is_floating_point, test_Char());
			AXE_TEST_CASE(Test_TypeTraits__is_floating_point, test_Misc());
		}

		static void test_is_arithmetic() {
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Int());
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Float());
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Bool());
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Char());
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Enum());
			AXE_TEST_CASE(Test_TypeTraits__is_arithmetic, test_Misc());
		}
	}; // TypeCategories

	#if 0
	#pragma mark------------ Type transformations --------------
	#endif
	struct TypeTransformations : public StaticAbstructClass {
		static void test_enable_if() {
			AXE_TEST_CASE(Test_TypeTraits__enable_if, test());
		}

		static void test_disable_if() {
			AXE_TEST_CASE(Test_TypeTraits__disable_if, test());
		}

		static void test_conditional() {
			AXE_TEST_CASE(Test_TypeTraits__conditional, test());
		}

		static void test_decay() {
			AXE_TEST_CASE(Test_TypeTraits__decay, test());
		}
	}; // TypeTransformations
};

} // namespace axe

void test_TypeTraits() {
	using H = axe::_test_TypeTraits_Helper;

#if 0 // single test
	using T = H::TypeTransformations;
	T::test_decay();
#else
	{
		using T = H::Core;
		T::test_integral_constant();
	}

	{
		using T = H::TypeCategories;
		T::test_is_const();
		T::test_is_void();
		T::test_is_integral();
		T::test_is_floating_point();
		T::test_is_arithmetic();
	}

	{
		using T = H::TypeTransformations;
		T::test_enable_if();
		T::test_disable_if();
		T::test_conditional();
		T::test_decay();
	}
#endif
}