#include "RenderFence.h"

namespace axe {

const TypeInfo* RenderFence::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderFence";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

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
