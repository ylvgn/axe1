#pragma once

#include "Mutex.h"

namespace axe {

#if 0
#pragma mark ========= CondVar ============
#endif
class CondVar : private NonCopyable {
	using This = CondVar;
public:
	using Locked = typename Mutex::ScopedLock;
	using Mutex = Locked::Mutex;

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
	#if AXE_OS_WINDOWS_VISTA_OR_LATER
		using Impl = ::CONDITION_VARIABLE;
	#else
		class Impl : public NonCopyable {
		public:
			Impl();
			~Impl();

			void signal();
			void broadcast();
			::BOOL timedWait(Locked& locked, ::DWORD waitMilliseconds);

		private:
			union {
				struct
				{
					::HANDLE _signalEvent;
					::HANDLE _broadcastEvent;
				};
				::HANDLE _events[2];
			};
			struct MData
			{
				int waitCount	   = 0;
				int broadcastCount = 0;
			};
			LockProtected<CondVar::Mutex, MData> _mdata;
		};
	#endif
#else
	using Impl = pthread_cond_t;
#endif

private:
#if AXE_OS_WINDOWS
	::BOOL _timedWait(Locked& locked, ::DWORD waitMilliseconds);
#endif

	Impl _c;
}; // CondVar


#if 0
#pragma mark ========= CondVarProtected ============
#endif
template <class VALUE>
class CondVarProtected : public NonCopyable {
public:
	using Locked = CondVar::Locked;
	using Mutex  = Locked::Mutex;
	using Value  = VALUE;

	class ScopedLock : public Locked {
		using Base = typename Locked;
	public:
		ScopedLock(Mutex& mutex, Value& value, CondVar& cv) noexcept
			: Base(mutex), _value(&value), _cv(&cv) {}

		ScopedLock(ScopedLock&& r) noexcept
			: Base(AXE_MOVE(r)), _value(r._value), _cv(r._cv)
		{
			r._value = nullptr;
			r._cv	 = nullptr;
		}

		void wait()
		{
			if (_cv && _mutex) {
				_cv->wait(*this);
			}
		}

		template <class Predicate>
		void wait(Predicate pred) {
			if (_cv && _mutex) {
				_cv->wait(*this, pred);
			}
		}

		Value* operator->()  { return _value; }
		operator Value*()    { return _value; }

		Value* value() const { return _value; }

	private:
		Value*	 _value = nullptr;
		CondVar* _cv	= nullptr;
	}; // ScopedLock

	ScopedLock scopedLock() { return ScopedLock(_mutex, _value, _cv); }

	void broadcast() {
		_cv.broadcast();
	}

	AXE_INLINE void notify_all() { broadcast(); }

private:
	Mutex	_mutex;
	Value	_value;
	CondVar _cv;
}; // CondVarProtected

} // namespace axe