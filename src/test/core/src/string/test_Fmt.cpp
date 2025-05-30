#include <axe_core/base/UnitTest.h>
#include <axe_core/string/Fmt.h>

namespace axe {

class Test_Fmt : public UnitTestBase {
public:	
	void test_string() {
		String s = "hello";
		AXE_DUMP_VAR(s);
	}
};

} // namespace 

void test_Fmt() {
	using namespace axe;
	AXE_TEST_CASE(Test_Fmt, test_string());
}
