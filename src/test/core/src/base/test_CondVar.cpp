#include <axe_core/base/UnitTest.h>
#include <axe_core/atomic/CondVar.h>
#include <axe_core/atomic/Mutex.h>
#include <axe_core/atomic/Atomic_Common.h>

namespace axe {

class Test_CondVar : public UnitTestBase {
	using This = Test_CondVar;
public:
	static CondVar s_cv;
	static Mutex   s_mutex;
	static int	   s_cnt;

	~Test_CondVar() {
		// Never manually unlock 's_mutex' when using scopedLock/lock_guard (RAII handles it)
	}

	static void _s_thread_main_waits() {
		auto scopedLock = s_mutex.scopedLock();

		ThreadUtil::Log("Waiting...");
		s_cv.wait(scopedLock, [] { return This::s_cnt >= 1; });
		ThreadUtil::Log("...Finished waiting. i >= 1");
		This::s_cnt += 1;
	}

	static void _s_thread_main_signals()
	{
		axe_sleep(1000);
		{
			auto scopedLock = s_mutex.scopedLock();
			ThreadUtil::Log("Notifying... And supposed nothing change");
		}
		s_cv.broadcast();

		axe_sleep(1000);
		{
			auto scopedLock = s_mutex.scopedLock();
			This::s_cnt = 1;
			ThreadUtil::Log("changed s_cnt == 1, then Notifying again...");
		}
		s_cv.broadcast();
	}

	void test() {
		This::s_cnt = 0;
		Vector<::std::thread, 4> threads;
		threads.emplace_back(&This::_s_thread_main_waits);
		threads.emplace_back(&This::_s_thread_main_waits);
		threads.emplace_back(&This::_s_thread_main_waits);
		threads.emplace_back(&This::_s_thread_main_signals);

		for (auto& thread : threads) {
			thread.join();
		}

		AXE_TEST_CHECK(This::s_cnt == threads.size());
	}
}; // Test_CondVar


CondVar	Test_CondVar::s_cv;
Mutex	Test_CondVar::s_mutex;
int		Test_CondVar::s_cnt = 0;

} // namespace axe


void test_CondVar() {
	using namespace axe;

	AXE_TEST_CASE(Test_CondVar, test());
}
