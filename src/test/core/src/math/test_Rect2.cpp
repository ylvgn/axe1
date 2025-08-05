#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Rect2.h>

namespace axe {

template<class Rect2>
class Test_Rect2 : public UnitTestBase {
public:
	using T = typename Rect2::ElementType;

	void test_area()
	{
		{
			Rect2 rc(T(1), T(2), T(3), T(4));
			AXE_TEST_CHECK(rc.area() == 12);
		}

		{
			Rect2 rc(T(1), T(2), T(0), T(4));
			AXE_TEST_CHECK(rc.area() == 0);
		}
	}
}; // Test_Rect2<Rect2>

template <>
class Test_Rect2<void> : public UnitTestBase {
public:
	template <class T>
	void test_perimeter()
	{
		{
			Rect2<T> rc(T(1), T(2), T(3), T(4));
			AXE_TEST_CHECK(rc.perimeter() == 14);
		}

		{
			Rect2<T> rc(T(1), T(2), T(0), T(4));
			AXE_TEST_CHECK(rc.perimeter() == 8);
		}
	}
}; // Test_Rect2<>

} // namespace axe


#define AXE_TEST_RECT2_CASE(...) \
	AXE_TEST_CASE(Test_Rect2 < Rect2i >, __VA_ARGS__); \
	AXE_TEST_CASE(Test_Rect2 < Rect2f >, __VA_ARGS__); \
//----


void test_Rect2() {
	using namespace axe;

	AXE_TEST_RECT2_CASE(test_area());

	AXE_TEST_CASE(Test_Rect2<void>, test_perimeter<int>());
	AXE_TEST_CASE(Test_Rect2<void>, test_perimeter<float>());
}
