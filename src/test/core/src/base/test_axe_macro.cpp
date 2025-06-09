#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_axe_macro : public UnitTestBase {
public:
	int calledCount = 0;
	int funcToRunOnce() {
		AXE_RUN_ONCE(++calledCount);
		return calledCount;
	}

	void test_AXE_RUN_ONCE()
	{
		AXE_TEST_CHECK(calledCount == 0);
		AXE_TEST_CHECK(funcToRunOnce() == 1);
		AXE_TEST_CHECK(calledCount == 1);
		AXE_TEST_CHECK(funcToRunOnce() == 1);
		AXE_TEST_CHECK(funcToRunOnce() == 1);

		_not_work_for_AXE_RUN_ONCE_pattern();
	}

	void _not_work_for_AXE_RUN_ONCE_pattern()
	{
		int badCalledCount = 0;

		AXE_RUN_ONCE(++badCalledCount); AXE_TEST_CHECK(badCalledCount == 1);
		AXE_RUN_ONCE(++badCalledCount); AXE_TEST_CHECK(badCalledCount == 2);
		AXE_RUN_ONCE(++badCalledCount); AXE_TEST_CHECK(badCalledCount == 3);
	}

	void test_AXE_ASSERT_ONCE() {
		auto sa = ScopedAction_make(
			[&]() { Error::s_setEnableAssertion(false); },
			[&]() { Error::s_setEnableAssertion(true); }
		);

		_test_AXE_ASSERT_ONCE();
		_test_AXE_ASSERT_ONCE();
	}

	void _test_AXE_ASSERT_ONCE() {
		AXE_ASSERT_ONCE("--------- test_AXE_ASSERT_ONCE ---------", 1 > 2, "");
	}

}; // Test_axe_macro

} // namespace axe


void test_axe_macro() {
	using namespace axe;

	//AXE_TEST_CASE(Test_axe_macro, test_AXE_RUN_ONCE());
	AXE_TEST_CASE(Test_axe_macro, test_AXE_ASSERT_ONCE());
}
