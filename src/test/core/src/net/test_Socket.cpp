#include <axe_core/base/UnitTest.h>
#include <axe_core/net/Socket.h>

namespace axe {

class Test_Socket : public UnitTestBase {
public:
	
	void test_resolveIPv4() {
		Vector<int> a;


		IPv4 ip;
		ip.resolve("localhost");
		AXE_DUMP_VAR(ip);
		AXE_TEST_CHECK(ip == IPv4(127,0,0,1));
		AXE_TEST_CHECK_SLIENT(ip == IPv4(127,0,0,1));
	}
};

} // namespace 

void test_Socket() {
	using namespace axe;
	AXE_TEST_CASE(Test_Socket, test_resolveIPv4());
}
