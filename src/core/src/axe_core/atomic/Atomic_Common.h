#pragma once

#include <axe_core/base/axe_base.h>
#include "../string/Fmt.h"

namespace axe {

#define ThreadPriority_ENUM_LIST(E) \
	E(Unknown, )                    \
	E(Highest, )                    \
	E(High, )                       \
	E(Normal, )                     \
	E(Low, )                        \
	E(Lowest, )                     \
	E(Idle, )                       \
//----
AXE_ENUM_CLASS(ThreadPriority, u8)


#if 0
#pragma mark ========= ThreadId_Native ============
#pragma mark ========= ThreadAffinityMask ============
#pragma mark ========= kThreadId_Native_Null ============
#endif
#if AXE_OS_WINDOWS
	using ThreadAffinityMask = ::DWORD_PTR;

class ThreadId_Native {
	using This = ThreadId_Native;
public:
	using NativeHandle = ::HANDLE;

	ThreadId_Native() = default;
	ThreadId_Native(NativeHandle handle_, ::DWORD id_)
		: handle(handle_), id(id_) {}

	operator NativeHandle() const { return handle; }

	NativeHandle handle = nullptr;
	::DWORD		 id = 0;
}; // ThreadId_Native
constexpr ThreadId_Native kThreadId_Native_Null = ThreadId_Native();

#elif AXE_OS_MACOSX || AXE_OS_IOS
	using ThreadAffinityMask = integer_t;
	using ThreadId_Native	 = pthread_t;

	const ThreadId_Native kThreadId_Native_Null = nullptr;
#else
	using ThreadAffinityMask = cpu_set_t;
	using ThreadId_Native	 = pthread_t;

	const ThreadId_Native kThreadId_Native_Null = 0;
#endif


#if 0
#pragma mark ========= ThreadId ============
#endif
class ThreadId {
	using This = ThreadId;
public:
	using NativeHandle = ThreadId_Native;

	static ThreadId s_current();

	explicit operator bool() const { return _v != kThreadId_Native_Null; }

	bool operator== (const This& r) const { return _v == r._v; }
	bool operator!= (const This& r) const { return _v != r._v; }

	void reset() { _v = kThreadId_Native_Null; }

	void setToCurrentThread() { *this = s_current(); }
	bool isCurrentThread() const;

	NativeHandle nativeHandle() { return _v; }

	void onFormat(fmt::format_context& ctx) const;

friend class Thread;
protected:
	NativeHandle _v = kThreadId_Native_Null;
}; // ThreadId
AXE_FORMATTER(ThreadId)


#if 0
#pragma mark ========= ThreadUtil ============
#endif
class ThreadUtil : public StaticClass {
public:

	template <class... Args>
	static AXE_INLINE void Log(Args&&... args)
	{
		AXE_LOG("Thread {}: {}", ThreadId::s_current(), AXE_FORWARD(args)...);
	}

#if 1 // C++11
	static AXE_INLINE void yield() { ::std::this_thread::yield(); }

#elif AXE_OS_WINDOWS
	static AXE_INLINE void yield() { ::YieldProcessor(); }

#elif AXE_OS_LINUX || AXE_OS_IOS || AXE_OS_MACOSX
	static AXE_INLINE void yield() { ::cpu_relax(); }

#elif AXE_COMPILER_GCC
	static AXE_INLINE void yield() { __asm__("pause"); }

#elif 1// pthread
	static AXE_INLINE void yield() { ::pthread_yield(void); }
#else
	#error
#endif // yield

#if AXE_OS_WINDOWS
	static AXE_INLINE bool setAffinity(ThreadId& threadId, ThreadAffinityMask mask) {
		if (!threadId) return false;
		auto result = ::SetThreadAffinityMask(threadId.nativeHandle(), mask);
		return 0 != result;
	}
#else
	#error TODO ThreadUtil::setAffinity
#endif // setAffinity
};

AXE_STATIC_ASSERT_NO_MEMBER_CLASS(ThreadUtil);

} // namespace axe