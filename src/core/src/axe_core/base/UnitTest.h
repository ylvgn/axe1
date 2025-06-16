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

#define AXE_TEST_CHECK(EXPR)		AXE_TEST_CHECK_IMPL(AXE_LOC, true,  bool(EXPR), #EXPR)
#define AXE_TEST_CHECK_SLIENT(EXPR) AXE_TEST_CHECK_IMPL(AXE_LOC, false, bool(EXPR), #EXPR)

#define AXE_TEST_CALL_TREATED_AS_SUCC(EXPR) \
	do { \
		{ EXPR; } \
		AXE_LOG("[ Passed ] [{}]", #EXPR); \
	} while (false) \
//----

#define AXE_TEST_NOEXCEPT_CALL(EXPR) \
	do { \
		try { EXPR; } \
		catch (...) { AXE_LOG("\t Caught throw: EXPR=[{}]", #EXPR); } \
	} while (false)	\
//----

namespace axe {

bool AXE_TEST_CHECK_IMPL(const SrcLoc& loc, bool verbose, bool success, const char* msg);

class UnitTestBase : public NonCopyable {
public:
};

} // namespace axe