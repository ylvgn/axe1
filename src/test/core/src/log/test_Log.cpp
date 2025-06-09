#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_Log : public UnitTestBase {
public:

	void test_TODO() {
		int AXE_TODO_calledCount = 0;
		AXE_TEST_CHECK(AXE_TODO_calledCount == 0);

		AXE_TODO("{} - {}", ++AXE_TODO_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_TODO_calledCount == 1);
		AXE_TODO("{} - {}", ++AXE_TODO_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_TODO_calledCount == 2);
		
		_AXE_TODO_work_for_per_function_scoped(AXE_TODO_calledCount);
	}

	void _AXE_TODO_work_for_per_function_scoped(int& AXE_TODO_calledCount){
		AXE_TODO("{} - {}", ++AXE_TODO_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_TODO_calledCount == 3);
		AXE_TODO("{} - {}", ++AXE_TODO_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_TODO_calledCount == 4);
	}

	void test_AXE_WARN_ONCE() {
		int AXE_WARN_ONCE_calledCount = 0;
		AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 0);

		AXE_WARN_ONCE("{} - {}", ++AXE_WARN_ONCE_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 1);
		AXE_WARN_ONCE("{} - {}", ++AXE_WARN_ONCE_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 2);

		_AXE_WARN_ONCE_work_for_per_function_scoped(AXE_WARN_ONCE_calledCount);
	}

	void _AXE_WARN_ONCE_work_for_per_function_scoped(int& AXE_WARN_ONCE_calledCount) {
		AXE_WARN_ONCE("{} - {}", ++AXE_WARN_ONCE_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 3);
		AXE_WARN_ONCE("{} - {}", ++AXE_WARN_ONCE_calledCount, AXE_FUNC_NAME_SZ); AXE_TEST_CHECK(AXE_WARN_ONCE_calledCount == 4);
	}

}; // Test_Log

} // namespace axe


void test_Log()
{
	using namespace axe;

	AXE_TEST_CASE(Test_Log, test_TODO());
	AXE_TEST_CASE(Test_Log, test_AXE_WARN_ONCE());

	AXE_ASSERT(1 > 2);

	AXE_DUMP_VAR(1);
}
