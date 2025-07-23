#pragma once

namespace axe {

class ScopedLock_Dummy {};

template <class... MUTEXES>
class AXE_NODISCARD ScopedLock : public NonCopyable {
public:
	using Dummy = ScopedLock_Dummy;

	explicit ScopedLock(MUTEXES&... mtxes)
		: _mtxes(&mtxes...)
	{
		AXE_TODO("make custom impl for std::lock");
		::std::lock(mtxes...);
	}

	explicit ScopedLock(Dummy&, MUTEXES&... mtxes) noexcept
		: _mtxes(&mtxes...)
	{
		// construct but don't lock
	}

	explicit ScopedLock(ScopedLock&& r) noexcept
		: _mtxes(AXE_MOVE(r._mtxes))
	{
		_mtxesCount	  = r._size();
		r._mtxes      = {};
		r._mtxesCount = 0;
	}

	~ScopedLock() noexcept { unlock(); }

	void lock(MUTEXES&... mtxes) {
		auto rhsMtxes = Tuple_make(&AXE_FORWARD(mtxes)...);
		if (_mtxes == rhsMtxes)
			return;
		_lock(rhsMtxes);
	}

	void unlock() {
		_mtxes.forEach([this](auto index, const auto& pMtx) {
			if (index >= _size())
				return;
			pMtx->unlock();
		});
	}

	Tuple<MUTEXES*...> mutexes() { return _mtxes; }

private:
	void _lock(Tuple<MUTEXES* ...>& mtxes) {
		_unlock();
		mtxes.forEach([](auto index, const auto& pMtx) {
			pMtx->lock();
		});
		_mtxes = mtxes;
		_mtxes._mtxesCount = mtxes.size();
	}

	AXE_INLINE size_t _size() const {
		return _mtxesCount.has_value() ? _mtxesCount.value() : _mtxes.size();
	}

protected:
	Tuple<MUTEXES* ...> _mtxes;
	Opt<size_t>			_mtxesCount;
}; // ScopedLock<ARGS...>

template <class... MUTEXES> AXE_NODISCARD
inline ScopedLock<MUTEXES...> ScopedLock_make(MUTEXES&... _mtxes) { return ScopedLock<MUTEXES...>(AXE_FORWARD(_mtxes)...); }


template <class MUTEX>
class AXE_NODISCARD ScopedLock<MUTEX> : public NonCopyable {
public:
	using Dummy = ScopedLock_Dummy;
    using Mutex = MUTEX;

    explicit ScopedLock(MUTEX& mutex) noexcept
		: _mutex(&mutex)
	{
		mutex.lock();
    }

    explicit ScopedLock(Dummy&, MUTEX& mutex) noexcept
		: _mutex(&mutex)
	{
		// construct but don't lock
	}

	explicit ScopedLock(ScopedLock&& r) noexcept
		: _mutex(r._mutex)
	{
		r._mutex = nullptr;
	}

    ~ScopedLock() noexcept {
		unlock();
    }

	void lock(MUTEX& mutex) {
		if (_mutex != &mutex) {
			unlock();
			mutex.lock();
			_mutex = &mutex;
		}
	}

	void unlock()
	{
		if (_mutex) {
			_mutex->unlock();
			_mutex = nullptr;
		}
	}

	Mutex* mutex() { return _mutex; }

protected:
	Mutex* _mutex = nullptr;
}; // ScopedLock<T>


template <>
class AXE_NODISCARD ScopedLock<> : public NonCopyable {
public:
	using Dummy = ScopedLock_Dummy;

	explicit ScopedLock() = default;
	explicit ScopedLock(Dummy&) noexcept {}
}; // ScopedLock<>

} // namespace axe