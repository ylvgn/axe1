#include "RenderFence.h"

namespace axe {

RenderFence::RenderFence(RenderFence_CreateDesc& desc) noexcept
	: Base(desc)
{
}

bool RenderFence::isDone() {
	if (_signaled)
		return true;

	if (onCheckCompleted())
	{
		_signaled = 1;
		return true;
	}
	return false;
}

} // namespace axe
