#pragma once

#include <axe_core/base/axe_base.h>
#include <axe_core/log/Log.h>

#define	AXE_TEST_CASE(TestClass, TestFunc) \
	do{ \
		AXE_LOG("\n[--TEST_CASE--] " #TestClass "." #TestFunc "\n"); \
		TestClass testObj; \
		testObj.TestFunc; \
	}while(false)
//----

#define AXE_TEST_CHECK(a)			AXE_TEST_CHECK_IMPL(AXE_LOC, true,  bool(a), #a)
#define AXE_TEST_CHECK_SLIENT(a)	AXE_TEST_CHECK_IMPL(AXE_LOC, false, bool(a), #a)

namespace axe {

bool AXE_TEST_CHECK_IMPL(const SrcLoc& loc, bool verbose, bool success, const char* msg);

class UnitTestBase : public NonCopyable {
public:
};

} // namespace axe