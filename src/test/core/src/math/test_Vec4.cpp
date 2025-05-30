#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Vec4.h>

namespace axe {

template<class Vec>
class Test_Vec4 : public UnitTestBase {
public:	
	using T = typename Vec::ElementType;

	void operator_test1() {
		Vec t = Tuple4<T>(4,5,6,7);
		AXE_TEST_CHECK(t == Vec(4,5,6,7));

		Vec a(1,2,3,4);
		Vec b(2,4,6,8);

		AXE_TEST_CHECK(a + b == Vec( 3, 6, 9, 12));
		AXE_TEST_CHECK(a - b == Vec(-1, -2, -3, -4));
		AXE_TEST_CHECK(a * b == Vec( 2, 8, 18, 32));
		AXE_TEST_CHECK(a / b == Vec( 0.5, 0.5, 0.5, 0.5));

		{ Vec c = a; c += b; AXE_TEST_CHECK(c == a + b); }
		{ Vec c = a; c -= b; AXE_TEST_CHECK(c == a - b); }
		{ Vec c = a; c *= b; AXE_TEST_CHECK(c == a * b); }
		{ Vec c = a; c /= b; AXE_TEST_CHECK(c == a / b); }

		T s = 2;
		AXE_TEST_CHECK(a + s == Vec(3,4,5,6));
		AXE_TEST_CHECK(a - s == Vec(-1,0,1,2));
		AXE_TEST_CHECK(a * s == Vec(2,4,6,8));
		AXE_TEST_CHECK(a / s == Vec(0.5, 1, 1.5, 2));

		{ Vec c = a; c += s; AXE_TEST_CHECK(c == a + s); }
		{ Vec c = a; c -= s; AXE_TEST_CHECK(c == a - s); }
		{ Vec c = a; c *= s; AXE_TEST_CHECK(c == a * s); }
		{ Vec c = a; c /= s; AXE_TEST_CHECK(c == a / s); }

	}
};

} // ----- end of namespace -----

#define AXE_TEST_Vec4_CASE(...) \
	AXE_TEST_CASE(Test_Vec4< Vec4f_Basic>, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Vec4< Vec4f_SSE  >, __VA_ARGS__); \
	\
	AXE_TEST_CASE(Test_Vec4< Vec4d_Basic>, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Vec4< Vec4d_SSE  >, __VA_ARGS__); \
//---

void test_Vec4() {
	using namespace axe;
 	AXE_TEST_Vec4_CASE(operator_test1());
}
