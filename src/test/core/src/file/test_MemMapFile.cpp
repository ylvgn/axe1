#include <axe_core/base/UnitTest.h>
#include <axe_core/file/MemMapFile.h>

namespace axe {

class Test_MemMapFile : public UnitTestBase {
public:	
	void test_open() {
		String filename = __FILE__;
		filename.append("_sample.txt");

		static const char* sample_sz = "1234ABCD";
		ByteSpan sample(reinterpret_cast<const u8*>(sample_sz), sizeof(sample_sz));

		MemMapFile mm;
		mm.open(filename);

		AXE_TEST_CHECK(sample == mm.span());
	}
};

} // namespace 

void test_MemMapFile() {
	using namespace axe;
	AXE_TEST_CASE(Test_MemMapFile, test_open());
}
