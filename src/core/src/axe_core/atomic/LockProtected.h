#pragma once

#include "ScopedLock.h"

namespace axe {

template <class MUTEX, class VALUE>
class LockProtected : public NonCopyable {
	using This = LockProtected;
public:
	using Mutex = MUTEX;
	using Value = VALUE;

	class ScopedLock : public Mutex::ScopedLock {
		using Base = typename Mutex::ScopedLock;
	public:
		ScopedLock(Mutex& mutex, Value& value) noexcept
			: Base(mutex)
		{
			_value = &value;
		}

		ScopedLock(ScopedLock&& r) noexcept
			: Base(AXE_MOVE(r))
		{
			_value	 = r._value;
			r._value = nullptr;
		}

		void unlock()
		{
			Base::unlock();
			_value = nullptr;
		}

		Value* operator->() { return _value; }
//		operator Value*  ()	{ return _value; }

		Value* value() const { return _value; }
		Mutex* mutex() const { return Base::mutex(); }
	private:
		Value* _value = nullptr;
	}; // ScopedLock

	template <class... Args>
	LockProtected(Args&&... args) : _value(AXE_FORWARD(args)...) {}

	AXE_NODISCARD ScopedLock scopedLock() {
		return ScopedLock(_mutex, _value);
	}

protected:
	Value _value;
	Mutex _mutex;
}; // LockProtected

} // namespace axe