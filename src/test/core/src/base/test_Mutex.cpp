#include <axe_core/atomic/Mutex.h>
#include <axe_core/base/UnitTest.h>

namespace axe
{

class Test_Mutex : public UnitTestBase
{
	using This = Test_Mutex;

public:
	static StringMap<String> s_pages;
	static Mutex			 s_pages_mutex;

	~Test_Mutex()
	{
		s_pages.clear();
		s_pages_mutex.unlock();
	}

	template <class SCOPEDLOCK>
	static void _s_test_single_lock_thread_main(StrView url)
	{
		axe_sleep(2000);
		auto seconds_since_epoch = ::std::chrono::duration_cast<::std::chrono::seconds>(
			::std::chrono::system_clock::now().time_since_epoch());

		TempString tmp(url);

		SCOPEDLOCK scoped(s_pages_mutex);
		s_pages[tmp.c_str()] = String::s_format("[{}]: {}", seconds_since_epoch.count(), url);
	}

	template <class SCOPEDLOCK>
	void test_single_lock()
	{
		Vector<::std::thread, 2> threads;
		threads.emplace_back(&This::_s_test_single_lock_thread_main<SCOPEDLOCK>, "http://foo");
		threads.emplace_back(&This::_s_test_single_lock_thread_main<SCOPEDLOCK>, "http://bar");

		for (auto& thread : threads)
		{
			thread.join();
		}

		for (const auto& [url, page] : s_pages)
		{
			AXE_DUMP_VAR(url, page);
		}
		AXE_TEST_CHECK(s_pages.size() == 2);
	}

	struct Employee
	{
		Employee(StrView id)
			: id(id) {}

		String partners() const
		{
			String ret	 = "Employee " + id + " has lunch partners: ";
			int	   count = 0;
			for (const auto& partner : lunch_partners)
			{
				ret += (count++ ? ", " : "") + partner;
			}
			return ret;
		}

		Vector<String> lunch_partners;
		Mutex		   m;
		String		   id;
	};

	static void s_send_mail(Employee&, Employee&)
	{
		// Simulate a time-consuming messaging operation
		axe_sleep(1000);
	}

	static void s_assign_lunch_partner(Employee& e1, Employee& e2)
	{
		AXE_LOG("{} and {} are waiting for locks", e1.id, e2.id);

		{
			// Use std::scoped_lock to acquire two locks without worrying about
			// other calls to assign_lunch_partner deadlocking us
			// and it also provides a convenient RAII-style mechanism

			ScopedLock lock(e1.m, e2.m);

			// Equivalent code 1 (using std::lock and std::lock_guard)
			// std::lock(e1.m, e2.m);
			// std::lock_guard<std::mutex> lk1(e1.m, std::adopt_lock);
			// std::lock_guard<std::mutex> lk2(e2.m, std::adopt_lock);

			// Equivalent code 2 (if unique_locks are needed, e.g. for condition variables)
			// std::unique_lock<std::mutex> lk1(e1.m, std::defer_lock);
			// std::unique_lock<std::mutex> lk2(e2.m, std::defer_lock);
			// std::lock(lk1, lk2);
			AXE_LOG("{} and {} got locks", e1.id, e2.id);
			e1.lunch_partners.push_back(e2.id);
			e2.lunch_partners.push_back(e1.id);
		}

		s_send_mail(e1, e2);
		s_send_mail(e2, e1);
	}

	void test_multi_lock()
	{
		// Ref: https://en.cppreference.com/w/cpp/thread/scoped_lock.html
		Employee alice("Alice"), bob("Bob"), christina("Christina"), dave("Dave");

		// Assign in parallel threads because mailing users about lunch assignments
		// takes a long time
		Vector<std::thread> threads;
		threads.emplace_back(&This::s_assign_lunch_partner, std::ref(alice), std::ref(bob));
		threads.emplace_back(&This::s_assign_lunch_partner, std::ref(christina), std::ref(bob));
		threads.emplace_back(&This::s_assign_lunch_partner, std::ref(christina), std::ref(alice));
		threads.emplace_back(&This::s_assign_lunch_partner, std::ref(dave), std::ref(bob));

		for (auto& thread : threads)
		{
			thread.join();
		}

		std::cout << alice.partners() << '\n'
				  << bob.partners() << '\n'
				  << christina.partners() << '\n'
				  << dave.partners() << '\n';
	}

}; // Test_Mutex

StringMap<String> Test_Mutex::s_pages;
Mutex			  Test_Mutex::s_pages_mutex;

} // namespace axe

void test_Mutex()
{
	using namespace axe;

	AXE_TEST_CASE(Test_Mutex, test_single_lock<::std::lock_guard<Mutex>>());
	AXE_TEST_CASE(Test_Mutex, test_single_lock<ScopedLock<Mutex>>());
	AXE_TEST_CASE(Test_Mutex, test_multi_lock());
}
