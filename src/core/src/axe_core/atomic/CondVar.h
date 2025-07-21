#pragma once

#include "Mutex.h"

namespace axe {

class CondVar : private NonCopyable {
	using This = CondVar;
public:
	using Locked = typename Mutex::ScopedLock;

	CondVar();
	~CondVar();

	void wait(Locked& locked);

	template <class Predicate>
	void wait(Locked& locked, Predicate pred) {
		while (!pred())
		{
			wait(locked);
		}
	}

	void signal();
	void broadcast();

	bool timedWait(Locked& locked, int waitMilliseconds);

	AXE_INLINE void notify_one() { signal(); }
	AXE_INLINE void notify_all() { broadcast(); }

#if AXE_OS_WINDOWS
	using NativeCondVar = ::CONDITION_VARIABLE;
#else
	using NativeCondVar = pthread_cond_t;
#endif

private:
#if AXE_OS_WINDOWS
	BOOL _timedWait(Locked& locked, ::DWORD waitMilliseconds);
#endif

	NativeCondVar _cv;
}; // CondVar

} // namespace axe