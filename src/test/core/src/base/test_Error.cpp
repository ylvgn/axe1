#include <axe_core/base/UnitTest.h>

#define AXE_TEST_NO_THROW_CALL(EXPR)            \
	do                                          \
	{                                           \
		try                                     \
		{                                       \
			EXPR;                               \
		}                                       \
		catch (...)                             \
		{                                       \
			AXE_LOG("\t ignored throw: {}", #EXPR); \
		}                                       \
	} while (false) \
//----

namespace axe {

class Test_Error : public UnitTestBase {
public:
	void test_AXE_THROW() {
		AXE_TEST_NO_THROW_CALL( AXE_THROW(""));
		AXE_TEST_NO_THROW_CALL( AXE_THROW("unsupported") );
		AXE_TEST_NO_THROW_CALL( AXE_THROW("{}", 123) );
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
		AXE_ASSERT(false);
	}

	void test_AXE_FATAL_ASSERT() {
		AXE_FATAL_ASSERT(false);
	}
};

} // namespace axe


void test_Error() {
	using namespace axe;

	auto sa = ScopedAction_make(
		[&]() { Error::s_setEnableAssertion(false); },
		[&]() { Error::s_setEnableAssertion(true); }
	);

	AXE_TEST_CASE(Test_Error, test_AXE_THROW());
	AXE_TEST_CASE(Test_Error, test_AXE_ASSERT_ONCE());
	AXE_TEST_CASE(Test_Error, test_AXE_ASSERT());
//	AXE_TEST_CASE(Test_Error, test_AXE_FATAL_ASSERT());
}
