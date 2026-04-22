#include "Thread.h"
#include "../string/UtfUtil.h"
#include "../base/Error.h"
#include "../string/Fmt.h"

#if AXE_OS_WINDOWS
	#include "../native_ui/win32/NativeUI_Win32_Common.h"
#else
	// TODO
#endif

AXE_VC_WARNING_PUSH()
AXE_VC_WARNING_DISABLE(5039) // warning C5039: pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.

namespace axe {

Thread::Thread() noexcept {
	_ctor();
}

Thread::~Thread() {
	join();
}

void Thread::start(CreateDesc& desc) {
	_create(desc);
	_start();
}

void Thread::_invokeFunc() {
	if (!_entry)
		return;

	try {
		_entry();
	} catch (Error& err) {
		AXE_LOG_ERROR("{}: {}", *this, err);
		throw;
	}
}

void Thread::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}({})", _name.empty() ? "axeThread" : _name, _threadId);
}

#if 0
#pragma mark ========= Windows ============
#endif
#if AXE_OS_WINDOWS

void Thread::_ctor() {
	_affinity = 0;
}

void Thread::_create(CreateDesc& desc) {
	_name = desc.name;

	auto stackSize = static_cast<SIZE_T>(desc.stackSize);

	if (stackSize < 0)
		AXE_THROW;

	if (_threadId)
		AXE_THROW;

	_entry = desc.entry;

	::LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr;
	AXE_TODO("Default security: lpThreadAttributes");

	::DWORD dwCreationFlags = CREATE_SUSPENDED;
	::HANDLE h = ::CreateThread(lpThreadAttributes
							  , stackSize
							  , reinterpret_cast<LPTHREAD_START_ROUTINE>(&Thread::s_proc)
							  , this
							  , dwCreationFlags
							  , nullptr);
	if (!h)
		AXE_WIN32_THROWIF_LAST_ERROR();

	_threadId._v = ThreadId_Native(h, ::GetThreadId(h));

	if (_name) {
		TempStringW tmpDesc;
		UtfUtil::convert(tmpDesc, _name);
		AXE_WIN32_THROWIF_HRESULT_ERROR(::SetThreadDescription(h, tmpDesc.c_str())); // Win32 makes an internal copy of the string.
	}
}

void Thread::_start() {
	AXE_ASSERT(_threadId);

	if (_priority != Priority::Normal)
		_setPriority(_priority);

	if (_affinity)
		_setAffinity(_affinity);

	bool needSetAffinity = false;
#if AXE_OS_MACOSX || AXE_OS_IOS || AXE_OS_WINDOWS
	if (_affinity)
		needSetAffinity = true;
#else
	if (::CPU_COUNT(&_affinity))
		needSetAffinity = true;
#endif

	if (needSetAffinity) {
		_setAffinity(_affinity);
	}

	::ResumeThread(_threadId.nativeHandle());
}

DWORD WINAPI Thread::s_proc(LPVOID p) {
	static_cast<Thread*>(p)->_invokeFunc();
	return 0;
}

void Thread::join() {
	if (!_threadId)
		return;

	::WaitForSingleObject(_threadId.nativeHandle(), INFINITE);
	detach();
}

void Thread::detach() {
	if (!_threadId)
		return;

	::CloseHandle(_threadId.nativeHandle());
	_threadId.reset();
}

void Thread::setPriority(Priority p) {
	AXE_TODO("");
}

void Thread::setAffinity(const AffinityMask& mask) {
	_setAffinity(mask);
}

void Thread::_setPriority(Priority p) {
	AXE_TODO("");
}

void Thread::_setAffinity(const AffinityMask& mask) {
	Util::setAffinity(_threadId, mask);
}

#if 0
#pragma mark ========= pthread ============
#endif
#else

#error "pthread TODO"

void* Thread::s_proc(void* p) {
	static_cast<axThread*>(p)->_invokeFunc();
	return 0;
}

#endif

} // namespace axe

AXE_VC_WARNING_POP()
