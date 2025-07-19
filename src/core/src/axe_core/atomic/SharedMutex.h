#pragma once

#include "LockProtected.h"

namespace axe {

class SharedMutex : public NonCopyable {
	using This = SharedMutex;
	using Impl = ::std::shared_mutex;
public:
	bool	tryLock	()		{ return _m.try_lock(); }
	void	lock	()		{ _m.lock(); }
	void	unlock	()		{ _m.unlock(); }

	bool	tryLockRead()	{ return _m.try_lock_shared(); }
	void	lockRead()		{ _m.lock_shared(); }
	void	unlockRead()	{ _m.unlock_shared(); }

private:
	Impl _m;
}; // SharedMutex

template <class VALUE> using SharedMutexProtected = typename LockProtected<SharedMutex, VALUE>;

} // namespace axe
