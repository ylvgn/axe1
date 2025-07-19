#pragma once

//#include "Mutex.h"

namespace axe {

class CondVar : public NonCopyable {
	using Impl = ::std::condition_variable;
public:
	//using Locked = Mutex::ScopedLock; // TODO

	using Locked = ::std::unique_lock<std::mutex>;

	void broadcast() { _m.notify_all(); }
	void signal() { _m.notify_one(); }

	void wait(Locked& locked) { _m.wait(locked); }

	bool timedWait(Locked& locked, int waitMilliseconds)
	{
		_m.wait_for(locked, ::std::chrono::milliseconds(waitMilliseconds));
		return true;
	}

private:
	Impl _m;
}; // CondVar

} // namespace axe