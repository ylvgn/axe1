#pragma once

#include "Atomic_Common.h"

namespace axe {

class Thread_CreateDesc {
public:
	using Func = AXE_FUNC<void ()>;
	using AffinityMask  = ThreadAffinityMask;

	StrView		 name;
	Func		 entry = nullptr;
	int			 stackSize = 0;
	AffinityMask affinity;
}; // Thread_CreateDesc


class Thread : public NonCopyable {
public:
	using Priority		= ThreadPriority;
	using AffinityMask  = ThreadAffinityMask;
	using Util			= ThreadUtil;

	using CreateDesc	= Thread_CreateDesc;
	using Func			= typename CreateDesc::Func;

	Thread() noexcept;
	virtual ~Thread();

	void start(CreateDesc& desc);
	void join	();
	void detach	();
	void setPriority(Priority p);
	void setAffinity(const AffinityMask& mask);

	ThreadId threadId() const { return _threadId; }

	void onFormat(fmt::format_context& ctx) const;

private:
	void _ctor();
	void _setPriority(Priority p);
	void _setAffinity(const AffinityMask& mask);
	void _create(CreateDesc& desc);
	void _start();
	void _invokeFunc();

#if AXE_OS_WINDOWS
	static DWORD WINAPI s_proc(LPVOID param);
#else
	static DWORD void* s_proc(void* param);
#endif

	String		 _name;
	ThreadId	 _threadId;
	Func		 _entry;
	Priority	 _priority = Priority::Normal;
	AffinityMask _affinity;

}; // Thread
AXE_FORMATTER(Thread)

} // namespace axe