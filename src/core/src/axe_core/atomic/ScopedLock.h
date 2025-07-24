#pragma once

namespace axe {

class ScopedLock_LockedAlready {};
class ScopedLock_TryLock {};

template <class... MUTEXES>
class AXE_NODISCARD ScopedLock : public NonCopyable {
public:
	using LockedAlready = ScopedLock_LockedAlready;
	using TryLock		= ScopedLock_TryLock;

	explicit ScopedLock(MUTEXES&... mtxes)					noexcept : _mtxes(&mtxes...), _isLock(true)  { ::std::lock(mtxes...); }
	explicit ScopedLock(LockedAlready&, MUTEXES&... locked) noexcept : _mtxes(&locked...), _isLock(true) { /*do nothing, auto unlock by RAII*/ }
	explicit ScopedLock(TryLock&, MUTEXES&... mtxes)		noexcept : _mtxes(&mtxes...), _isLock(false) { tryLock(); }

	~ScopedLock() noexcept { unlock(); }

#if AXE_CPLUSPLUS_17
	void unlock() noexcept {
		if (!isLocked())
			return;
		::eastl::apply([](auto&... mtxes) {
			(..., mtxes->unlock() ); // fold expression, equivalent to "(m1.unlock(), (m2.unlock(), (m3.unlock())))"
		}, _mtxes);
		_isLock = false;
	}
#else
	void unlock() noexcept {
		if (!isLocked())
			return;
		_mtxes.forEachReverse([](auto index, const auto& pMtx) {
			pMtx->unlock();
		});
		_isLock = false;
	}
#endif

#if AXE_CPLUSPLUS_17
	bool tryLock() {
		unlock();
		_isLock = ::eastl::apply([](auto&... mtxes) {
			return ::std::try_lock(*mtxes...);
		}, _mtxes) == -1;
		return _isLock;
	}
#else
	struct _interalTryLockHelper : public StaticAbstructClass {
		template <typename... MUTEXES>
		static int std_try_lock(Tuple<MUTEXES*...>& mtxes) {
			return s_tryLock(mtxes, IndexSequenceFor<MUTEXES...>{});
		}

	private:
		template <typename... MUTEXES, int... Indices>
		static int s_tryLock(Tuple<MUTEXES*...>& mtxes, IntSequence<Indices...>)
		{
			return ::std::try_lock(*::eastl::get<Indices>(mtxes)...);
		}
	};

	bool tryLock()
	{
		using H = _interalTryLockHelper;
		unlock();
		_isLock = H::std_try_lock(_mtxes) == -1;
		return _isLock;
	}
#endif

	explicit operator bool() const { return isLocked(); }

	bool	 isLocked()		 const { return _isLock; }

private:
	Tuple<MUTEXES* ...> _mtxes;
	bool			    _isLock : 1;
}; // ScopedLock<ARGS...>

template <class MUTEX>
class AXE_NODISCARD ScopedLock<MUTEX> : public NonCopyable {
public:
    using Mutex = MUTEX;

	using LockedAlready = ScopedLock_LockedAlready;
	using TryLock		= ScopedLock_TryLock;

	ScopedLock() = default;
	explicit ScopedLock(ScopedLock&& r)						noexcept : _mutex(r._mutex)		{ r._mutex = nullptr; }
    explicit ScopedLock(MUTEX& mutex)						noexcept : _mutex(&mutex)		{ mutex.lock(); }
    explicit ScopedLock(LockedAlready&, MUTEX& lockedMutex) noexcept : _mutex(&lockedMutex) { /* do nothing, auto unlock by RAII*/ }
	explicit ScopedLock(const TryLock&, MUTEX& mutex) { tryLock(mutex); }

    ~ScopedLock() noexcept { unlock(); }

	void lock(MUTEX& mutex) {
		if (_mutex != &mutex) {
			unlock();
			mutex.lock();
			_mutex = &mutex;
		}
	}

	bool tryLock(MUTEX& mutex) { return _tryLock(mutex); }

	void unlock() {
		if (_mutex) {
			_mutex->unlock();
			_mutex = nullptr;
		}
	}

	MUTEX* detach() noexcept {
		MUTEX* o = _mutex;
		_mutex = nullptr;
		return o;
	}

	MUTEX* mutex() noexcept { return _mutex; }

	bool isLocked() const { return _mutex; }
	explicit operator bool() const { return isLocked(); }

protected:
	bool _tryLock(MUTEX& mutex) {
		unlock();
		if (mutex.tryLock()) {
			_mutex = &mutex;
			_mutex->lock();
			return true;
		}
		return false;
	}

	MUTEX* _mutex = nullptr;
}; // ScopedLock<T>

template <>
class AXE_NODISCARD ScopedLock<> : public NonCopyable {
public:
	using LockedAlready = ScopedLock_LockedAlready;

	explicit ScopedLock() = default;
	explicit ScopedLock(LockedAlready&) noexcept {}
}; // ScopedLock<>


template <class... MUTEXES> AXE_NODISCARD
inline ScopedLock<MUTEXES...> ScopedLock_make(MUTEXES&... mtxes) { return ScopedLock<MUTEXES...>(AXE_FORWARD(mtxes)...); }

} // namespace axe