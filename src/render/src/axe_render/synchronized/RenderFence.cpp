#include "RenderFence.h"

namespace axe {

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
