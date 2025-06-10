#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_Log : public UnitTestBase {
public:
	void test_TODO() {
		int AXE_TODO_calledCount = 0;
		AXE_TEST_CHECK(AXE_TODO_calledCount == 0);

		_wrapFuncToRunOnce_AXE_TODO(AXE_TODO_calledCount); AXE_TEST_CHECK(AXE_TODO_calledCount == 1);
		_wrapFuncToRunOnce_AXE_TODO(AXE_TODO_calledCount); AXE_TEST_CHECK(AXE_TODO_calledCount == 1);
	}

	void _wrapFuncToRunOnce_AXE_TODO(int& AXE_TODO_calledCount) {
		AXE_TODO("{} - {}", ++AXE_TODO_calledCount, AXE_FUNC_NAME_SZ);
	}

	void test_AXE_WARN_ONCE() {
		int AXE_WARN_ONCE_calledCount = 0;
		AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 0);

		_wrapFuncToRunOnce_AXE_WARN_ONCE(AXE_WARN_ONCE_calledCount); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 1);
		_wrapFuncToRunOnce_AXE_WARN_ONCE(AXE_WARN_ONCE_calledCount); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 1);

		_AXE_WARN_ONCE_another_foo(AXE_WARN_ONCE_calledCount);
	}

	void _AXE_WARN_ONCE_another_foo(int& AXE_WARN_ONCE_calledCount) {
		_wrapFuncToRunOnce_AXE_WARN_ONCE(AXE_WARN_ONCE_calledCount); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 1);
		_wrapFuncToRunOnce_AXE_WARN_ONCE(AXE_WARN_ONCE_calledCount); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 1);
	}

	void _wrapFuncToRunOnce_AXE_WARN_ONCE(int& AXE_WARN_ONCE_calledCount) {
		AXE_WARN_ONCE("{} - {}", ++AXE_WARN_ONCE_calledCount, AXE_FUNC_NAME_SZ);
	}
}; // Test_Log

} // namespace axe


void test_Log()
{
	using namespace axe;

	AXE_TEST_CASE(Test_Log, test_TODO());
	AXE_TEST_CASE(Test_Log, test_AXE_WARN_ONCE());
}
