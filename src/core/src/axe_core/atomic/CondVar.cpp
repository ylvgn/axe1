#include "CondVar.h"

namespace axe {

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

bool CondVar::timedWait(Locked& locked, int waitMilliseconds) {
	if (waitMilliseconds < 0) {
		AXE_ASSERT(false);
		waitMilliseconds = 0;
	}
	return _timedWait(locked, static_cast<::DWORD>(waitMilliseconds));
}

#if AXE_OS_WINDOWS_VISTA_OR_LATER
CondVar::CondVar() {
	::InitializeConditionVariable(&_c);
}

CondVar::~CondVar() {
	/*Do Not Need to explicitly destroy CONDITION_VARIABLE*/
}

void CondVar::signal()		{ ::WakeConditionVariable(&_c); }
void CondVar::broadcast()	{ ::WakeAllConditionVariable(&_c);}

void CondVar::wait(Locked& locked) {
	_timedWait(locked, INFINITE);
}

::BOOL CondVar::_timedWait(Locked& locked, ::DWORD dwMilliseconds) {
	auto* m = locked.mutex();
	return ::SleepConditionVariableCS(&_c, m->nativeMutex(), dwMilliseconds); // Windows OS atomicity does: Re-locks the CRITICAL_SECTION before ::SleepConditionVariableCS returns.
}
#else

CondVar::CondVar()  {}
CondVar::~CondVar() {}
void CondVar::signal()												{ _c.signal();}
void CondVar::broadcast()											{ _c.broadcast(); }
void CondVar::wait(Locked& locked)									{ _timedWait(locked, INFINITE); }
::BOOL CondVar::_timedWait(Locked& locked, ::DWORD waitMilliseconds)  { return _c.timedWait(locked, waitMilliseconds); }

CondVar::Impl::Impl() {
	AXE_ASSERT(reinterpret_cast<void*>(&_signalEvent) == reinterpret_cast<void*>(&_events[0]));
	AXE_ASSERT(reinterpret_cast<void*>(&_broadcastEvent) == reinterpret_cast<void*>(&_events[1]));

	_signalEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!_signalEvent)
		AXE_THROW();

	_broadcastEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!_broadcastEvent)
		AXE_THROW();
}

CondVar::Impl::~Impl() {
	if (_signalEvent)
		::CloseHandle(_signalEvent);

	if (_broadcastEvent)
		::CloseHandle(_broadcastEvent);
}

void CondVar::Impl::signal() {
	::SetEvent(_signalEvent);
}

void CondVar::Impl::broadcast() {
	auto data = _mdata.scopedLock();

	if (data->waitCount == 0)
		return;

	data->broadcastCount = data->waitCount;

	::SetEvent(_broadcastEvent);
}

::BOOL CondVar::Impl::timedWait(Locked& locked, ::DWORD waitMilliseconds) {
	{
		auto data = _mdata.scopedLock();
		data->waitCount++;
	}

	auto* mutex = locked.mutex();
	AXE_ASSERT(mutex != nullptr);

	mutex->unlock();
	::DWORD ret = ::WaitForMultipleObjects(2, _events, FALSE, waitMilliseconds);
	mutex->lock();

	{
		auto data = _mdata.scopedLock();
		data->waitCount--;

		switch(ret) {
			case WAIT_TIMEOUT:
				return FALSE;

			case WAIT_OBJECT_0: // _signalEvent
				return TRUE;

			case WAIT_OBJECT_0+1: { // _broadcastEvent
				data->broadcastCount--;

				if (data->broadcastCount <= 0) {
					// I'm the last one 
					::ResetEvent(_broadcastEvent);
				}
				return TRUE;
			}break;

			case WAIT_ABANDONED_0:
			case WAIT_ABANDONED+1: {
				AXE_ASSERT(false);
				return FALSE;
			} break;
		}
	}

	AXE_ASSERT(false);
	return FALSE;
}
#endif

#if 0
#pragma mark ========= pthread ============
#endif
#else

CondVar::CondVar() {
	pthread_cond_init(&_c, nullptr);
}

CondVar::~CondVar() {
	pthread_cond_destroy(&_c);
}

void CondVar::signal() {
	pthread_cond_signal(&_c);
}

void CondVar::broadcast() {
	pthread_cond_broadcast(&_c);
}

void CondVar::wait(Locked& locked) {
	auto* m = locked.mutex();
	pthread_cond_wait(&_c, m->nativeMutex());
}

bool CondVar::timedWait(Locked& locked, int waitMilliseconds) {
	auto* m = locked.mutex();

	timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) < 0) {
		AXE_THROW();
	}

	ts.tv_sec  += waitMilliseconds / 1000;
	ts.tv_nsec += waitMilliseconds % 1000;
		
	int ret = pthread_cond_timedwait(&_c, m->nativeMutex(), &ts);
	switch (ret) {
		case 0:			return true;
		case ETIMEDOUT: return false;
		case EINVAL: AXE_THROW();
	}
	return false;
}

#endif

} // namespace axe