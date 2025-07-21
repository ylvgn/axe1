#include "CondVar.h"

namespace axe {

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

#if AXE_OS_WINDOWS_VISTA_OR_LATER
CondVar::CondVar() {
	::InitializeConditionVariable(&_cv);
}

CondVar::~CondVar() {
	/*Do Not Need to explicitly destroy CONDITION_VARIABLE*/
}

void CondVar::signal()		{ ::WakeConditionVariable(&_cv); }
void CondVar::broadcast()	{ ::WakeAllConditionVariable(&_cv);}

void CondVar::wait(Locked& locked) {
	_timedWait(locked, INFINITE);
}

bool CondVar::timedWait(Locked& locked, int waitMilliseconds) {
	if (waitMilliseconds < 0) {
		AXE_ASSERT(false);
		waitMilliseconds = 0;
	}
	return _timedWait(locked, static_cast<::DWORD>(waitMilliseconds));
}

BOOL CondVar::_timedWait(Locked& locked, DWORD dwMilliseconds) {
	auto* m = locked.mutex();
	return ::SleepConditionVariableCS(&_cv, m->nativeMutex(), dwMilliseconds);
}
#else
	#error only support Windows Vista or later version
#endif

#if 0
#pragma mark ========= pthread ============
#endif
#else

CondVar::CondVar() {
	pthread_cond_init(&_cv, nullptr);
}

CondVar::~CondVar() {
	pthread_cond_destroy(&_cv);
}

void CondVar::signal() {
	pthread_cond_signal(&_cv);
}

void CondVar::broadcast() {
	pthread_cond_broadcast(&_cv);
}

void CondVar::wait(Locked& locked) {
	auto* m = locked.mutex();
	pthread_cond_wait(&_cv, m->nativeMutex());
}

bool CondVar::timedWait(Locked& locked, int waitMilliseconds) {
	auto* m = locked.mutex();

	timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) < 0) {
		AXE_THROW();
	}

	ts.tv_sec  += waitMilliseconds / 1000;
	ts.tv_nsec += waitMilliseconds % 1000;
		
	int ret = pthread_cond_timedwait(&_cv, m->nativeMutex(), &ts);
	switch (ret) {
		case 0:			return true;
		case ETIMEDOUT: return false;
		case EINVAL: AXE_THROW();
	}
	return false;
}

#endif
} // namespace axe