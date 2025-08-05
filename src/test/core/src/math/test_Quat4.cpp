#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Quat4.h>

namespace axe {

template<class Quat4>
class Test_Quat4 : public UnitTestBase {
public:
	using T = typename Quat4::ElementType;

	void test_lerp() {
		Quat4 q1(T(1), T(3), T(7), T(10));
		Quat4 q2(T(5), T(5), T(5), T(4 ));

		{
			auto q = Math::lerp(q1, q2, T(0.5));
			AXE_TEST_CHECK(q == Quat4(T(3), T(4), T(6), T(7)));
		}
		{
			auto q = Math::lerp(q1, q2, 0.5); // double weight
			AXE_TEST_CHECK(q == Quat4(T(3), T(4), T(6), T(7)));
		}
	}
}; // Test_Quat4<Quat4>


template <>
class Test_Quat4<void> : public UnitTestBase {
public:

	template <class T>
	void test_lerpf() {
		Quat4<T> q1(T(1), T(2.5), T(3), T(4.5));
		Quat4<T> q2(T(5), T(5),   T(5), T(5)  );

		{
			auto q = Math::lerp(q1, q2, T(0.5));
			AXE_TEST_CHECK(q == Quat4<T>(T(3), T(3.75), T(4), T(4.75)));
		}
		{
			auto q = Math::lerp(q1, q2, 0.5); // double weight
			AXE_TEST_CHECK(q == Quat4<T>(T(3), T(3.75), T(4), T(4.75)));
		}
	}
}; // Test_Quat4<>

} // namespace axe

#define AXE_TEST_QUAT4_CASE(...) \
	AXE_TEST_CASE(Test_Quat4< Quat4f >, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Quat4< Quat4d >, __VA_ARGS__); \
//----

void test_Quat4() {
	using namespace axe;
	AXE_TEST_QUAT4_CASE(test_lerp());

	AXE_TEST_CASE(Test_Quat4<void>, test_lerpf<float>());
	AXE_TEST_CASE(Test_Quat4<void>, test_lerpf<double>());
}
