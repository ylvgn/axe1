#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Quat4.h>

namespace axe {

template<class Quat4>
class Test_Quat4 : public UnitTestBase {
public:
	using T = typename Quat4::ElementType;

	void test_lerp() {

		Quat4 q1(T(1), T(2.5), T(3), T(4.5));
		Quat4 q2(T(5), T(5),   T(5), T(5)  );

		{
			auto q = Math::lerp(q1, q2, T(0.5));
			AXE_DUMP_VAR(q.x, q.y, q.z, q.w);
		}
		{
			auto q = Math::lerp(q1, q2, 0.5); // double weight
			AXE_DUMP_VAR(q.x, q.y, q.z, q.w);
		}
	}
};

} // ----- end of namespace -----

#define AXE_TEST_QUAT_CASE(...) \
	AXE_TEST_CASE(Test_Quat4< Quat4f >, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Quat4< Quat4d >, __VA_ARGS__); \
//---

void test_Quat4() {
	using namespace axe;
	AXE_TEST_QUAT_CASE(test_lerp());
	return;
}
