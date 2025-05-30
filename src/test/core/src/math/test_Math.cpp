#include <axe_core/base/UnitTest.h>
#include <axe_core/math/Math.h>

namespace axe {

class Test_Math : public UnitTestBase {
public:	
	void nextPow2() {
		AXE_TEST_CHECK(0 == Math::nextPow2(0));
		AXE_TEST_CHECK(1 == Math::nextPow2(1));
		AXE_TEST_CHECK(2 == Math::nextPow2(2));
		AXE_TEST_CHECK(4 == Math::nextPow2(3));
		AXE_TEST_CHECK(8 == Math::nextPow2(5));
	}

	void nextPow16() {
		AXE_TEST_CHECK(0  == Math::nextPow16(0));
		AXE_TEST_CHECK(16 == Math::nextPow16(1));
		AXE_TEST_CHECK(16 == Math::nextPow16(15));
		AXE_TEST_CHECK(32 == Math::nextPow16(17));
		AXE_TEST_CHECK(32 == Math::nextPow16(32));
		AXE_TEST_CHECK(64 == Math::nextPow16(33));
	}

	void alignTo() {
		// signed int
		AXE_TEST_CHECK(0 == Math::alignTo(1, 0));
		AXE_TEST_CHECK(0	== Math::alignTo(0, 0));
		AXE_TEST_CHECK(0	== Math::alignTo(-1, 0));
		AXE_TEST_CHECK(2	== Math::alignTo(2, 1));
		AXE_TEST_CHECK(0	== Math::alignTo(0, 4));
		AXE_TEST_CHECK(0	== Math::alignTo(0, 7));
		AXE_TEST_CHECK(8	== Math::alignTo(2, 8));
		AXE_TEST_CHECK(12	== Math::alignTo(4, 12));
		AXE_TEST_CHECK(24	== Math::alignTo(16, 12));
		AXE_TEST_CHECK(64	== Math::alignTo(64, 32));
		AXE_TEST_CHECK(96	== Math::alignTo(65, 32));
		AXE_TEST_CHECK(128	== Math::alignTo(65, 64));
		AXE_TEST_CHECK(-128 == Math::alignTo(-65, -64));
		AXE_TEST_CHECK(-32	== Math::alignTo(-31, -32));

		// unsigned int
		AXE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(8)));
		AXE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(2)));
		AXE_TEST_CHECK(u8(17)	 == Math::alignTo(u8(8), u8(17)));
		AXE_TEST_CHECK(u8(34)	 == Math::alignTo(u8(18), u8(17)));
		AXE_TEST_CHECK(u8(8)	 == Math::alignTo(u8(8), u8(8)));
		AXE_TEST_CHECK(u32(200)  == Math::alignTo(u32(199), u32(200)));
		AXE_TEST_CHECK(u32(256)  == Math::alignTo(u32(199), u32(256)));
		AXE_TEST_CHECK(u32(512)  == Math::alignTo(u32(500), u32(256)));
		AXE_TEST_CHECK(u32(1025) == Math::alignTo(u32(1024), u32(1025)));
		AXE_TEST_CHECK(u32(1024) == Math::alignTo(u32(1024), u32(1024)));
	}
};

} // ----- end of namespace -----

void test_Math() {
	using namespace axe;
	AXE_TEST_CASE(Test_Math, nextPow2());
	AXE_TEST_CASE(Test_Math, nextPow16());
	AXE_TEST_CASE(Test_Math, alignTo());
}
