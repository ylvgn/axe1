#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_axe_macro : public UnitTestBase {
public:
	int AXE_RUN_ONCE_calledCount = 0;
	int funcTo_AXE_RUN_ONCE() { // wrap a custom function then use AXE_RUN_ONCE
		AXE_RUN_ONCE(++AXE_RUN_ONCE_calledCount);
		return AXE_RUN_ONCE_calledCount;
	}

	void test_AXE_RUN_ONCE() {
		AXE_TEST_CHECK(AXE_RUN_ONCE_calledCount == 0);
		AXE_TEST_CHECK(funcTo_AXE_RUN_ONCE() == 1);
		AXE_TEST_CHECK(AXE_RUN_ONCE_calledCount == 1);
		AXE_TEST_CHECK(funcTo_AXE_RUN_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_RUN_ONCE() == 1);

		_AXE_RUN_ONCE_not_work_in_same_function_scope();
	}

	void _AXE_RUN_ONCE_not_work_in_same_function_scope() {
		int calledCount = 0;
		AXE_TEST_CHECK(calledCount == 0);
		AXE_RUN_ONCE(++calledCount); AXE_TEST_CHECK(calledCount == 1);
		AXE_RUN_ONCE(++calledCount); AXE_TEST_CHECK(calledCount == 2);
		AXE_RUN_ONCE(++calledCount); AXE_TEST_CHECK(calledCount == 3);
	}
}; // Test_axe_macro

} // namespace axe


void test_axe_macro() {
	using namespace axe;

	AXE_TEST_CASE(Test_axe_macro, test_AXE_RUN_ONCE());
}
