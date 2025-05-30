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

}

void run_all_test() {
	
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
