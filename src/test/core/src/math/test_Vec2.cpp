#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Vec2.h>

namespace axe {

template<class Vec>
class Test_Vec2 : public UnitTestBase {
public:	
	using T = typename Vec::ElementType;

	void operator_test1() {
		Vec t = Tuple2<T>(4,5);
		AXE_TEST_CHECK(t == Vec(4,5));

		Vec a(1,2);
		Vec b(2,4);

		AXE_TEST_CHECK(a + b == Vec( 3, 6));
		AXE_TEST_CHECK(a - b == Vec(-1, -2));
		AXE_TEST_CHECK(a * b == Vec( 2, 8));
		AXE_TEST_CHECK(a / b == Vec( 0.5, 0.5));

		{ Vec c = a; c += b; AXE_TEST_CHECK(c == a + b); }
		{ Vec c = a; c -= b; AXE_TEST_CHECK(c == a - b); }
		{ Vec c = a; c *= b; AXE_TEST_CHECK(c == a * b); }
		{ Vec c = a; c /= b; AXE_TEST_CHECK(c == a / b); }

		T s = 2;
		AXE_TEST_CHECK(a + s == Vec(3,4));
		AXE_TEST_CHECK(a - s == Vec(-1,0));
		AXE_TEST_CHECK(a * s == Vec(2,4));
		AXE_TEST_CHECK(a / s == Vec(0.5, 1));

		{ Vec c = a; c += s; AXE_TEST_CHECK(c == a + s); }
		{ Vec c = a; c -= s; AXE_TEST_CHECK(c == a - s); }
		{ Vec c = a; c *= s; AXE_TEST_CHECK(c == a * s); }
		{ Vec c = a; c /= s; AXE_TEST_CHECK(c == a / s); }

	}
};

} // ----- end of namespace -----

#define AXE_TEST_VEC2_CASE(...) \
	AXE_TEST_CASE(Test_Vec2< Vec2f_Basic>, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Vec2< Vec2f_SSE  >, __VA_ARGS__); \
	\
	AXE_TEST_CASE(Test_Vec2< Vec2d_Basic>, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Vec2< Vec2d_SSE  >, __VA_ARGS__); \
//---

void test_Vec2() {
	using namespace axe;
 	AXE_TEST_VEC2_CASE(operator_test1());
}
