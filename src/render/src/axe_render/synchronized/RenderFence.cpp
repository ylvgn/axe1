#include "RenderFence.h"

namespace axe {

void RenderFence::create(RenderFence_CreateDesc& desc) {
	onCreate(desc);
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
