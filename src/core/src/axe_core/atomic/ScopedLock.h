#pragma once

namespace axe {

class ScopedLock_LockedAlready {};
class ScopedLock_TryLock {};

template <class... MUTEXES>
class AXE_NODISCARD ScopedLock : public NonCopyable {
public:
	using LockedAlready = ScopedLock_LockedAlready;
	using TryLock		= ScopedLock_TryLock;

	explicit ScopedLock(MUTEXES&... mtxes)					noexcept : _mtxes(&mtxes...), _isLocked(true)  { ::std::lock(mtxes...); }
	explicit ScopedLock(LockedAlready&, MUTEXES&... locked) noexcept : _mtxes(&locked...), _isLocked(true) { /*do nothing, auto unlock by RAII*/ }
	explicit ScopedLock(TryLock&, MUTEXES&... mtxes)		noexcept : _mtxes(&mtxes...), _isLocked(false) { tryLock(); }

	~ScopedLock() noexcept { unlock(); }

#if AXE_CPLUSPLUS_17
	void unlock() noexcept {
		if (!isLocked())
			return;
		::eastl::apply([](auto&... mtxes) {
			(..., mtxes->unlock() ); // fold expression, equivalent to "(m1.unlock(), (m2.unlock(), (m3.unlock())))"
		}, _mtxes);
		_isLocked = false;
	}
#else
	void unlock() noexcept {
		if (!isLocked())
			return;
		_mtxes.forEachReverse([](auto index, const auto& pMtx) {
			pMtx->unlock();
		});
		_isLocked = false;
	}
#endif

#if AXE_CPLUSPLUS_17
	bool tryLock() {
		unlock();
		_isLocked = ::eastl::apply([](auto&... mtxes) {
			return ::std::try_lock(*mtxes...);
		}, _mtxes) == -1;
		return _isLocked;
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
		_isLocked = H::std_try_lock(_mtxes) == -1;
		return _isLocked;
	}
#endif

	explicit operator bool() const { return isLocked(); }

	bool	 isLocked()		 const { return _isLocked; }

private:
	Tuple<MUTEXES* ...> _mtxes;
	bool			    _isLocked : 1;
}; // ScopedLock<ARGS...>


template <class MUTEX>
class AXE_NODISCARD ScopedLock<MUTEX> : public NonCopyable {
public:
    using Mutex = MUTEX;

	using LockedAlready = ScopedLock_LockedAlready;
	using TryLock		= ScopedLock_TryLock;

	ScopedLock() = default;
	explicit ScopedLock(ScopedLock && r)					noexcept : _mutex(r._mutex)		{ r._mutex = nullptr; }
    explicit ScopedLock(MUTEX& mutex)						noexcept : _mutex(&mutex)		{ mutex.lock(); }
    explicit ScopedLock(LockedAlready&, MUTEX& lockedMutex) noexcept : _mutex(&lockedMutex) { /* do nothing, auto unlock by RAII*/ }
	explicit ScopedLock(const TryLock&, MUTEX& mutex)		noexcept { tryLock(mutex); }

    ~ScopedLock() noexcept { unlock(); }

	void lock(MUTEX& mutex) noexcept {
		if (_mutex != &mutex) {
			unlock();
			mutex.lock();
			_mutex = &mutex;
		}
	}

	bool tryLock(MUTEX& mutex) noexcept { return _tryLock(mutex); }

	void unlock() noexcept {
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

	explicit operator bool() const { return isLocked(); }

	bool isLocked() const { return _mutex; }

protected:
	bool _tryLock(MUTEX& mutex) noexcept {
		unlock();
		if (mutex.tryLock()) {
			_mutex = &mutex;
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


template <class T0, class T1>
class ScopedLock2 : public NonCopyable {
public:
	using TryLock = ScopedLock_TryLock;

	ScopedLock2(ScopedLock2 && r) noexcept
		: _m0(AXE_MOVE(r._m0)), _m1(AXE_MOVE(r._m1)) {}

	explicit ScopedLock2(T0& t0, T1& t1) noexcept {
		if (&t0 < &t1) { //using address to decide the order
			_m0.lock(t0);
			_m1.lock(t1);
		}else{
			_m1.lock(t1);
			_m0.lock(t0);
		}
	}

	explicit ScopedLock2(const TryLock&, T0& t0, T1& t1) noexcept {
		if (&t0 < &t1) { //using address to decide the order
			_m0.tryLock(t0);
			_m1.tryLock(t1);
		}else{
			_m1.tryLock(t1);
			_m0.tryLock(t0);
		}
	}

	explicit operator bool() const { return isLocked(); }

	bool isLocked() const { return _m0.isLocked() && _m1.isLocked(); }

private:
	ScopedLock<T0> _m0;
	ScopedLock<T1> _m1;
}; // ScopedLock2<T>


template<class MUTEX> AXE_NODISCARD
inline ScopedLock<MUTEX> ScopedLock_make(MUTEX& m) { return ScopedLock<MUTEX>(m); }

template<class T0, class T1> AXE_NODISCARD
inline ScopedLock2<T0, T1> ScopedLock_make(T0& t0, T1& t1) { return ScopedLock2<T0, T1>(t0, t1); }

template <class... MUTEXES> AXE_NODISCARD
inline ScopedLock<MUTEXES...> ScopedLock_make(MUTEXES&... mtxes) { return ScopedLock<MUTEXES...>(AXE_FORWARD(mtxes)...); }

} // namespace axe