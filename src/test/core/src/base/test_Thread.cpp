#include <axe_core/base/UnitTest.h>

#include <axe_core/system/SystemInfo.h>
#include <axe_core/atomic/Thread.h>
#include <axe_core/atomic/Mutex.h>
#include <axe_core/atomic/CondVar.h>

#if AXE_OS_WINDOWS
	#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>
#endif

namespace axe {

class Test_Thread : public UnitTestBase {
	using This = Test_Thread;
public:
	#if AXE_OS_WINDOWS
		class MyHiResTimer {
		public:
			MyHiResTimer() noexcept {
				::LARGE_INTEGER f;
				::QueryPerformanceFrequency(&f);
				Win32Util::convert(_freq, f);
				reset();
			}

			void reset() { _tickCount = _getTick(); }
			double get() const { return static_cast<double>(_tickCount) / double(_freq); }
		private:
			u64 _getTick() {
				::LARGE_INTEGER v;
				::QueryPerformanceCounter(&v);
				uint64_t o;
				Win32Util::convert(o, v);
				return o;
			}

			u64 _freq;
			u64 _tickCount;
		}; // MyHiResTimer

		class MyStopWatch {
		public:
			MyStopWatch() noexcept {
				_start.reset();
				_last = _start;
			}

			void reset()			{ _start.reset(); }
			void resetToLastGet()	{ _start = _last; }
			double lastGet()		{ return _last.get() - _start.get(); }

			double get() {
				_last.reset();
				return lastGet();
			}
		private:
			MyHiResTimer _start;
			MyHiResTimer _last;
		}; // MyStopWatch

		class MyScopedStopWatch : public NonCopyable {
		public:
			MyScopedStopWatch(MyStopWatch& stopWatch) noexcept : _stopWatch(&stopWatch) { stopWatch.reset(); }
			~MyScopedStopWatch() noexcept {	 AXE_DUMP_VAR(_stopWatch->get()); }
		private:
			MyStopWatch* _stopWatch = nullptr;
		}; // MyScopedStopWatch

		AXE_NODISCARD static MyScopedStopWatch MyScopedStopWatch_make(MyStopWatch& stopWatch) { return MyScopedStopWatch(stopWatch); }
	#else
		class MyStopWatch {};
		AXE_NODISCARD static void MyScopedStopWatch_make(MyStopWatch& stopWatch) { AXE_TODO(""); }
	#endif // AXE_OS_WINDOWS

	static const int N = 10000000;
	static int		 s_cnt;

	static void _s_test_raceCondition_thread_main()
	{
		for (int i = 0; i < This::N * 2; i++) {
			s_cnt = s_cnt + 1;
		}
	}

	void test_raceCondition() {
		s_cnt = 0;
		{
			Thread t0;
			Thread_CreateDesc desc;
			desc.entry = &This::_s_test_raceCondition_thread_main;
			t0.start(desc);

			for (int i = 0; i < This::N; i++) {
				s_cnt = s_cnt + 1;
			}
		}

		AXE_DUMP_VAR(s_cnt);
		AXE_TEST_CHECK(s_cnt != This::N * 3);
	}

	static Mutex s_mutex;
	static void _test_scopedLock_thread_main() {
		for (int i = 0; i < This::N * 2; i++) {
			auto scoped = ScopedLock_make(s_mutex);
			s_cnt = s_cnt + 1;
		}
	}

	void test_scopedLock() {
		s_cnt = 0;
		s_mutex.unlock();

		{
			Thread			  t0;
			Thread_CreateDesc desc;
			desc.entry = &This::_test_scopedLock_thread_main;
			t0.start(desc);

			for (int i = 0; i < This::N; i++)
			{
				auto scoped = ScopedLock_make(s_mutex);
				s_cnt = s_cnt + 1;
			}
		}

		AXE_TEST_CHECK(s_cnt == This::N * 3);
	}

	const static i64 kPrimeStart  = 1000000000LL;
	const static int kThreadCount = 4;
	const static i64 kBatchSize   = 10;

	static bool s_isPrimeNumber(i64 n) {
		if (n < 1)
			return false;
		for (decltype(n) i = 2; i < n; i++) {
			if (n % i == 0)
				return false;
		}
		return true;
	}

	class MyPrimeNumberRequest002 {
		using This = MyPrimeNumberRequest002;

		Mutex		_mutex;
		Vector<i64> _primeNumbers;

		using Lock = ScopedLock<decltype(This::_mutex)>;
	public:
		void addResult(i64 v) {
			Lock lock(_mutex);
			_primeNumbers.push_back(v);
		}

		void finish()
		{
			Lock lock(_mutex);
			AXE_LOG("resultCount={}", _primeNumbers.size());
		}
	}; // MyPrimeNumberRequest002

	class MyPrimeNumberThread002 : private Thread {
		using This = MyPrimeNumberThread002;
		using Base = Thread;
	public:
		using Request = MyPrimeNumberRequest002;

		void run(Request& req, i64 id, i64 start, i64 batch) {
			_req	= &req;
			_id		= id;
			_start	= start;
			_batch	= batch;

			Thread_CreateDesc threadDescs;
			auto name = TempString::s_format("MyPrimeNumberThread002 - {}", id);
			threadDescs.name  = name;
			threadDescs.entry = [this]() { onThreadProc(); };

			Base::setAffinity(1LL << _id);
			Base::start(threadDescs);
		}

		void onThreadProc() {
//			AXE_DUMP_VAR(_id, _start, _batch);

			for (i64 i = 0; i < _batch; i++)
			{
				i64 v = _start + i;
				if (Test_Thread::s_isPrimeNumber(v)) {
					AXE_LOG("Thread {}: prime {}", _id, v);
					_req->addResult(v);
				}
			}
			AXE_LOG("Thread {}: ended", _id);
		}

		Request* _req	= nullptr;
		i64		 _id	= 0;
		i64		 _start = 0;
		i64		 _batch = 0;
	}; // MyPrimeNumberThread002


	class MyPrimeNumberRequest003 {
		using This = MyPrimeNumberRequest003;

		Mutex		_mutex;
		Vector<i64> _primeNumbers;

		i64 _start	 = 0;
		i64 _count	 = 0;
		i64 _current = 0;

		using Lock = ScopedLock<decltype(This::_mutex)>;
	public:

		void init(i64 start, i64 count) {
			_start	 = start;
			_count	 = count;
			_current = start;
		}

		i64 getNext() {
			Lock lock(_mutex);
			if (_current >= _start + _count)
				return 0;

			auto ret = _current;
			_current++;
			return ret;
		}

		void addResult(i64 v) {
			Lock lock(_mutex);
			_primeNumbers.push_back(v);
		}

		void finish() {
			Lock lock(_mutex);
			AXE_LOG("resultCount={}", _primeNumbers.size());
		}
	}; // MyPrimeNumberRequest003

	template <class REQUEST>
	class MyPrimeNumberThread003 : private Thread {
		using This = MyPrimeNumberThread003;
		using Base = Thread;
	public:
		using Request = REQUEST;

		void run(Request& req, i64 id) {
			_req	= &req;
			_id		= id;

			Thread_CreateDesc threadDescs;
			auto name = TempString::s_format("MyPrimeNumberThread003 - {}", id);
			threadDescs.name  = name;
			threadDescs.entry = [this]() { onThreadProc(); };

			Base::setAffinity(1LL << _id);
			Base::start(threadDescs);
		}

		void onThreadProc() {
//			AXE_DUMP_VAR(_id);

			for (;;) {
				i64 v = _req->getNext();
				if (v == 0)
					break;

				if (Test_Thread::s_isPrimeNumber(v)) {
					ThreadUtil::Log("CPU{}: prime: {}", _id, v);
					_req->addResult(v);
				}
			}
			ThreadUtil::Log("CPU{}: ended", _id);
		}

		Request* _req	= nullptr;
		i64		 _id	= 0;
	}; // MyPrimeNumberThread003

	class MyPrimeNumberRequest004 {
		using This = MyPrimeNumberRequest004;

		struct Request
		{
			i64 start	= 0;
			i64 count	= 0;
			i64 current = 0;
		};

		struct Result
		{
			Vector<i64> primeNumbers;
		};

		MutexProtected<Request> _request;
		MutexProtected<Result>	_result;

	public:
		void init(i64 start, i64 count) {
			auto req	 = _request.scopedLock();
			req->start   = start;
			req->current = start;
			req->count   = count;
		}

		i64 getNext() {
			auto req = _request.scopedLock();
			if (req->current >= req->start + req->count)
				return 0;

			auto ret = req->current;
			req->current++;
			return ret;
		}

		void addResult(i64 v) {
			auto res = _result.scopedLock();
			res->primeNumbers.push_back(v);
		}

		void finish() {
			auto res = _result.scopedLock();
			AXE_LOG("resultCount={}", res->primeNumbers.size());
		}
	}; // MyPrimeNumberRequest004

	void _test_primeNumber_singleThread() {
		auto scoped = MyScopedStopWatch_make(_stopWatch);

		ThreadId						   mainThreadId = ThreadId::s_current();
		int								   whichCpu		= 2;
		remove_cv_t<decltype(kPrimeStart)> resultCount	= 0;
		ThreadUtil::setAffinity(mainThreadId, static_cast<ThreadAffinityMask>(1LL << whichCpu));

		for (decltype(resultCount) i = 0; i < kBatchSize * kThreadCount; i++) {
			auto v = kPrimeStart + i;
			if (s_isPrimeNumber(v))
			{
				AXE_LOG("MainThread: prime {}", v);
				resultCount++;
			}
		}
		AXE_LOG("resultCount={}", resultCount);
	}

	void _test_primeNumber_multiThread_BatchMode() {
		auto					scoped = MyScopedStopWatch_make(_stopWatch);
		MyPrimeNumberRequest002 req;
		{
			MyPrimeNumberThread002 threads[kThreadCount];
			for (i64 i = 0; i < kThreadCount; i++)
			{
				i64 start = kPrimeStart + i * kBatchSize;
				threads[i].run(req, i, start, kBatchSize);
			}
		}
		req.finish();
	}

	template <class MyThreadT>
	void _test_primeNumber_multiThread_BossMode() {
		auto scoped = MyScopedStopWatch_make(_stopWatch);

		MyThreadT::Request req;
		req.init(kPrimeStart, kThreadCount * kBatchSize);
		{
			MyThreadT threads[kThreadCount];
			for (i64 i = 0; i < kThreadCount; i++)
			{
				threads[i].run(req, i);
			}
		}

		req.finish();
	}

	class MyPrimeNumberRequest005 {
		using This = MyPrimeNumberRequest005;

		struct Request
		{
			i64 start	= 0;
			i64 count	= 0;
			i64 current = 0;
		};

		struct Result
		{
			Vector<i64> primeNumbers;
			int			endedThread = 0;
		};

		CondVarProtected<Request> _request;
		CondVarProtected<Result>  _result;

	public:
		void init(i64 start, i64 count) {
			auto req	 = _request.scopedLock();
			req->start   = start;
			req->current = start;
			req->count   = count;
		}

		i64 getNext() {
			auto req = _request.scopedLock();
			if (req->current >= req->start + req->count)
				return 0;

			auto ret = req->current;
			req->current++;
			return ret;
		}

		void addResult(i64 v) {
			{
				auto res = _result.scopedLock();
				res->primeNumbers.push_back(v);
			}

			_result.notify_all(); // just for MainThread print some msg, and there no need notify_all is fine.
		}

		void finish() {
			auto res = _result.scopedLock();
			AXE_LOG("resultCount={}", res->primeNumbers.size());
		}

		void threadEnded()
		{
			{
				auto res = _result.scopedLock();
				res->endedThread++;
			}
			_result.notify_all();
		}

		void waitForEnd() {
			auto res = _result.scopedLock();
			for (;;)
			{
				AXE_LOG("MainThread: resultCount={}", res->primeNumbers.size());
				if (res->endedThread >= kThreadCount) {
					break;
				}
				res.wait();
			}

			AXE_LOG("waitForEnd is done");
		}
	}; // MyPrimeNumberRequest005


	class MyPrimeNumberThread005 : private Thread {
		using This = MyPrimeNumberThread005;
		using Base = Thread;
	public:
		using Request = MyPrimeNumberRequest005;

		void run(Request& req, i64 id) {
			_req	= &req;
			_id		= id;

			Thread_CreateDesc threadDescs;
			auto name = TempString::s_format("MyPrimeNumberThread005 - {}", id);
			threadDescs.name  = name;
			threadDescs.entry = [this]() { onThreadProc(); };

			Base::setAffinity(1LL << _id);
			Base::start(threadDescs);
		}

		void onThreadProc() {
//			AXE_DUMP_VAR(_id);

			for (;;) {
				i64 v = _req->getNext();
				if (v == 0) {
					break;
				}

				if (Test_Thread::s_isPrimeNumber(v)) {
					ThreadUtil::Log("CPU{}: prime: {}", _id, v);
					_req->addResult(v);
				}
			}

			ThreadUtil::Log("CPU{}: ended", _id);
			_req->threadEnded();
		}

		Request* _req	= nullptr;
		i64		 _id	= 0;
	}; // MyPrimeNumberThread005

	void _test_primeNumber_conditionVariable() {
		auto scoped = MyScopedStopWatch_make(_stopWatch);

		MyPrimeNumberRequest005 req;
		req.init(kPrimeStart, kThreadCount * kBatchSize);
		{
			MyPrimeNumberThread005 threads[kThreadCount];
			for (i64 i = 0; i < kThreadCount; i++) {
				threads[i].run(req, i);
			}
		}
		req.waitForEnd();
		req.finish();
	}

	void test_primeNumber() {
		static const int kCpuCount = SystemInfo::cpuCount();
		if (kThreadCount > kCpuCount) {
			AXE_LOG_WARN("[bypass] expect cpu proc count {}, but only got {}", kThreadCount, kCpuCount);
			return;
		}

		_test_primeNumber_singleThread();
		_test_primeNumber_multiThread_BatchMode();
		_test_primeNumber_multiThread_BossMode<MyPrimeNumberThread003<MyPrimeNumberRequest003>>();
		_test_primeNumber_multiThread_BossMode<MyPrimeNumberThread003<MyPrimeNumberRequest004>>();
		_test_primeNumber_conditionVariable();
	}

	class BankAccount {
		#if 0
			using Mutex = ::std::mutex;
			using ScopedLock = ::std::unique_lock<Mutex>;
		#else
			using Mutex = ::axe::Mutex;
			using ScopedLock = Mutex::ScopedLock;
		#endif

		void _sleep(int ms = 1000) { axe_sleep(ms); }
	public:
		void transfer(BankAccount& to, int amount) {
			//doTransfer_DeadLock(to, amount);
//			doTransfer_LockInOrder(to, amount);
//			doTransfer_LockInOrder2(to, amount);
			doTransfer_TryLock(to, amount);
		}

		void doTransfer_DeadLock(BankAccount& to, int amount) {
			ThreadUtil::Log("start transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));

			ThreadUtil::Log("lock {:p}", fmt::ptr(this));
			auto lockFrom = ScopedLock(_mutex);
			ThreadUtil::Log("locked {:p}", fmt::ptr(this));

			_sleep(2000);

			ThreadUtil::Log("lock {:p}", fmt::ptr(&to));
			auto lockTo = ScopedLock(to._mutex);
			ThreadUtil::Log("locked {:p}", fmt::ptr(&to));

			_sleep();
			_balance -= amount;
			to._balance += amount;

			ThreadUtil::Log("end transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));
		}

		void doTransfer_LockInOrder(BankAccount& to, int amount) {
			ThreadUtil::Log("start transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));

			if (this < &to)
			{
				ThreadUtil::Log("lock {:p}", fmt::ptr(this));
				auto lockFrom = ScopedLock(_mutex);
				ThreadUtil::Log("locked {:p}", fmt::ptr(this));

				_sleep(2000);

				ThreadUtil::Log("lock {:p}", fmt::ptr(&to));
				auto lockTo = ScopedLock(to._mutex);
				ThreadUtil::Log("locked {:p}", fmt::ptr(&to));
			}
			else
			{
				ThreadUtil::Log("lock {:p}", fmt::ptr(&to));
				auto lockTo = ScopedLock(to._mutex);
				ThreadUtil::Log("locked {:p}", fmt::ptr(&to));

				_sleep(2000);

				ThreadUtil::Log("lock {:p}", fmt::ptr(this));
				auto lockFrom = ScopedLock(_mutex);
				ThreadUtil::Log("locked {:p}", fmt::ptr(this));
			}

			_sleep();
			_balance    -= amount;
			to._balance += amount;

			ThreadUtil::Log("end transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));
		}

		void doTransfer_LockInOrder2(BankAccount& to, int amount) {
			ThreadUtil::Log("start transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));

			ThreadUtil::Log("lock {:p} {:p}", fmt::ptr(this), fmt::ptr(&to));
			auto scoped = ScopedLock_make(_mutex, to._mutex);
			ThreadUtil::Log("locked {:p} {:p}", fmt::ptr(this), fmt::ptr(&to));

			_sleep();
			_balance -= amount;
			to._balance += amount;

			ThreadUtil::Log("end transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));
		}
		
		void doTransfer_TryLock(BankAccount& to, int amount) {
			ThreadUtil::Log("start transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));

			auto myTryLock = ScopedLock::TryLock();
			for (;;)
			{
				ThreadUtil::Log("lock {:p}", fmt::ptr(this));
				auto lockFrom = ScopedLock(_mutex);
				ThreadUtil::Log("locked {:p}", fmt::ptr(this));

				_sleep(2000);

				ThreadUtil::Log("try lock {:p}", fmt::ptr(&to));
				auto lockTo = ScopedLock(myTryLock, to._mutex); // auto lockTo = ScopedLock(to._mutex, ::std::try_to_lock);

				if (!lockTo.isLocked()) // if (!lockTo.owns_lock())
				{
					lockFrom.unlock(); // <--- unlock 'lockFrom' before sleep
					ThreadUtil::Log("unlocked {:p}", fmt::ptr(this));
					_sleep();
					continue;
				}

				ThreadUtil::Log("locked {:p}", fmt::ptr(&to));

				_sleep();
				_balance -= amount;
				to._balance += amount;

				ThreadUtil::Log("end transfer {:p} -> {:p}", fmt::ptr(this), fmt::ptr(&to));
				break;
			}
		}

		int balance()
		{
			ScopedLock scoped(_mutex);
			return _balance;
		}

	private:
		Mutex _mutex;
		int	  _balance = 1000;
	};

	void test_deadLock() {
		{
			Thread thread0;
			Thread_CreateDesc thread0Desc;
			thread0Desc.entry = ([this]() {
				ThreadUtil::Log("thread0 start");
				accountA.transfer(accountB, 10);
			});
			thread0.start(thread0Desc);


			ThreadUtil::Log("MainThread start");
			accountB.transfer(accountA, 1);
		}

		AXE_TEST_CHECK(accountA.balance() == 1000 - 10 + 1);
		AXE_TEST_CHECK(accountB.balance() == 1000 + 10 - 1);
	}

	BankAccount accountA;
	BankAccount accountB;

	MyStopWatch _stopWatch;

}; // Test_Thread

int		Test_Thread::s_cnt = 0;
Mutex	Test_Thread::s_mutex;

} // namespace axe

void test_Thread() {
	using namespace axe;

	AXE_TEST_CASE(Test_Thread, test_raceCondition());
	AXE_TEST_CASE(Test_Thread, test_scopedLock());
	AXE_TEST_CASE(Test_Thread, test_primeNumber());
	AXE_TEST_CASE(Test_Thread, test_deadLock());
}
