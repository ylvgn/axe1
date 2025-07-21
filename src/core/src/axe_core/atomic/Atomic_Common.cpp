#include "Atomic_Common.h"

namespace axe {

#if 0
#pragma mark ========= ThreadId ============
#endif
bool ThreadId::isCurrentThread() const {
	return *this == s_current();
}

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

ThreadId ThreadId::s_current() {
	ThreadId o;
	auto h = ::GetCurrentThread();
	o._v = ThreadId::NativeHandle(h, ::GetThreadId(h));
	return o;
}

void ThreadId::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}", _v.id);
}

#if 0
#pragma mark ========= Apple ============
#endif
#elif AXE_OS_MACOSX || AXE_OS_IOS

ThreadId ThreadId::s_current() {
	ThreadId o;
	o._v = ::pthread_self();
	return o;
}

void ThreadId::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{:p}", fmt::ptr(static_cast<const void*>(_v)));
}

#if 0
#pragma mark ========= pthread ============
#endif
#else

ThreadId ThreadId::s_current() {
	ThreadId o;
	o._v = pthread_self();
	return o;
}

void ThreadId::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}", static_cast<u64>(_v));
}

#endif

} // namespace axe
