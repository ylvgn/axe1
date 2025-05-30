#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Mat4.h>

namespace axe {

template<class Mat4>
class Test_Mat4 : public UnitTestBase {
public:	
	using T = typename Mat4::ElementType;

	void operator_test1() {
		Mat4 m = Mat4::s_identity();
		m += 1;

		AXE_TEST_CHECK(m == Mat4({2,1,1,1}, 
								 {1,2,1,1},
								 {1,1,2,1},
								 {1,1,1,2}));

		AXE_DUMP_VAR(m);
	}
};

} // ----- end of namespace -----

#define AXE_TEST_MAT4_CASE(...) \
	AXE_TEST_CASE(Test_Mat4< Mat4f_Basic>, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Mat4< Mat4d_Basic>, __VA_ARGS__); \
	\
	AXE_TEST_CASE(Test_Mat4< Mat4f_SSE  >, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Mat4< Mat4d_SSE  >, __VA_ARGS__); \
//---

void test_Mat4() {
	using namespace axe;
 	AXE_TEST_MAT4_CASE(operator_test1());
}
