#include <axe_core/base/Error.h>
#include <axe_core/base/UnitTest.h>
#include <axe_core/log/Log.h>

// AXE_OPTIMIZE_OFF

#define	RUN_TEST( fn )	\
	AXE_LOG("\n===== " #fn " ================================\n"); \
	void fn(); \
	fn(); \
//----

namespace axe {

void run_temp_test() {
	RUN_TEST(test_Math);
}

void run_all_test() {
// base
	RUN_TEST(test_BinSerializer);

// file
	RUN_TEST(test_MemMapFile);

// math
	RUN_TEST(test_Math);
	RUN_TEST(test_Vec2);
	RUN_TEST(test_Vec3);
	RUN_TEST(test_Vec4);
	RUN_TEST(test_Quat4);
	RUN_TEST(test_Mat4);

// string
	RUN_TEST(test_Fmt);
	RUN_TEST(test_String);
	RUN_TEST(test_StrView);
	RUN_TEST(test_UtfUtil);

// net
	RUN_TEST(test_Socket);

// pointer
	RUN_TEST(test_WPtr);
}

int test_main() {
#if 0
	run_temp_test();
#else
	run_all_test();
#endif
	// TODO: check memory leak

	AXE_LOG("\n\n==== Program Ended ==== \n");
	return 0;
}

} // namespace

int main() {
	return axe::test_main();
}
