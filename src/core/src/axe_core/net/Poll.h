#pragma once

#include "../base/axe_base.h"

namespace axe {

enum class PollFlags : i16 {
	None	= 0,
	In		= POLLIN,
	Out		= POLLOUT,
	InOut	= POLLIN | POLLOUT,
	Error	= POLLERR,
	PRI		= POLLPRI,
	Hup		= POLLHUP,
	Invalid = POLLNVAL,
};

AXE_ENUM_BITWISE_OPERATOR(PollFlags)

#if AXE_OS_WINDOWS
	using PollFd = ::WSAPOLLFD;
	inline int Poll(Span<PollFd> fds, int timeoutMilliseconds) {
		int ret = ::WSAPoll(fds.data(), static_cast<ULONG>(fds.size()), timeoutMilliseconds);
		return ret;
	}
#else
	using PollFd = ::pollfd;
	inline int Poll(Span<PollFd> fds, int timeoutMilliseconds) {
		int ret = ::poll(fds.data(), fds.size(), timeoutMilliseconds);
		return ret;
	}
#endif


} // namespace
