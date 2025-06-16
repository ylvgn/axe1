#include <axe_core/base/UnitTest.h>

namespace axe {

class Test_Error : public UnitTestBase {
public:
	void test_AXE_THROW() {
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_TEST_NOEXCEPT_CALL(throw AXE_ERROR("")) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_TEST_NOEXCEPT_CALL(throw AXE_ERROR("unsupported")) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_TEST_NOEXCEPT_CALL(throw AXE_ERROR("{}", 123)) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_TEST_NOEXCEPT_CALL(AXE_THROW()) );
	}

	int AXE_ASSERT_ONCE_calledCount = 0;
	int funcTo_AXE_ASSERT_ONCE() {
		AXE_ASSERT_ONCE(++AXE_ASSERT_ONCE_calledCount > 2);
		return AXE_ASSERT_ONCE_calledCount;
	}

	void test_AXE_ASSERT_ONCE() {
		AXE_TEST_CHECK(AXE_ASSERT_ONCE_calledCount == 0);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(funcTo_AXE_ASSERT_ONCE() == 1);
		AXE_TEST_CHECK(AXE_ASSERT_ONCE_calledCount == 1);
	}

	void test_AXE_ASSERT() {
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_ASSERT(false) );

		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_ASSERT(1 > 2) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_ASSERT(1 < 2) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_ASSERT(1 == 2) );
	}

	void test_AXE_FATAL_ASSERT() {
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_FATAL_ASSERT(true) );
		AXE_TEST_CALL_TREATED_AS_SUCC( AXE_FATAL_ASSERT(false) );
	}
};

} // namespace axe


void test_Error() {
	using namespace axe;

	auto scopedDisabledAssert = ScopedAction_make(
		[&]() { Error::s_setEnableAssertion(false); },
		[&]() { Error::s_setEnableAssertion(true); }
	);

	auto scopedDisabledDebugBreak = ScopedAction_make(
		[&]() { Error::s_setEnableDebugBreak(false); },
		[&]() { Error::s_setEnableDebugBreak(true); }
	);

	AXE_TEST_CASE(Test_Error, test_AXE_THROW());
	AXE_TEST_CASE(Test_Error, test_AXE_ASSERT_ONCE());
	AXE_TEST_CASE(Test_Error, test_AXE_ASSERT());
//	AXE_TEST_CASE(Test_Error, test_AXE_FATAL_ASSERT());
}
