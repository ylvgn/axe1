#pragma once

#include "LockProtected.h"

namespace axe {

class Mutex : private NonCopyable {
	using This = Mutex;
public:
	using ScopedLock = ScopedLock<This>;

	AXE_INLINE  Mutex();
	AXE_INLINE  ~Mutex();

	AXE_INLINE void lock();
	AXE_INLINE bool tryLock();
	AXE_INLINE void unlock();

	AXE_NODISCARD ScopedLock scopedLock() { return ScopedLock(*this); }

#if AXE_OS_WINDOWS
	using NativeMutex = ::CRITICAL_SECTION;
#else
	using NativeMutex = pthread_mutex_t;
#endif

	AXE_INLINE bool try_lock() { return tryLock(); }

	NativeMutex* nativeMutex() { return &_m; }

private:
	NativeMutex _m;
}; // Mutex

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS
AXE_INLINE Mutex::Mutex()			{ ::InitializeCriticalSection(&_m); }
AXE_INLINE Mutex::~Mutex()			{ ::DeleteCriticalSection(&_m); }
AXE_INLINE void Mutex::lock()		{ ::EnterCriticalSection(&_m); }
AXE_INLINE bool Mutex::tryLock()	{ return ::TryEnterCriticalSection(&_m) != 0; }
AXE_INLINE void Mutex::unlock()		{ ::LeaveCriticalSection(&_m); }

#if 0
#pragma mark ========= pthread ============
#endif
#else

AXE_INLINE void Mutex::Mutex() {
	::pthread_mutexattr_t attr;
	::pthread_mutexattr_init(&attr);
	::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	::pthread_mutex_init(&_m, &attr);
	::pthread_mutexattr_destroy(&attr);
}
AXE_INLINE void Mutex::~Mutex()		{ ::pthread_mutex_destroy(&_m); }
AXE_INLINE void Mutex::lock()		{ ::pthread_mutex_lock(&_m); }
AXE_INLINE bool Mutex::tryLock()	{ return ::pthread_mutex_trylock(&_m) != 0; }
AXE_INLINE void Mutex::unlock()		{ return ::pthread_mutex_unlock(&_m); }

#endif

template <class VALUE> using MutexProtected = typename LockProtected<Mutex, VALUE>;

} // namespace axe
