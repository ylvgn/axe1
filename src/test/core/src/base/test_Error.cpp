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
};

} // namespace axe


void test_Error() {
	using namespace axe;

	auto sa = ScopedAction_make(
		[&]() { Error::s_setEnableAssertion(false); },
		[&]() { Error::s_setEnableAssertion(true); }
	);

	AXE_TEST_CASE(Test_Error, test_AXE_THROW());
}
