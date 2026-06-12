#pragma once

#include "../RenderDeviceObject.h"

namespace axe {

class RenderFence : public RenderDeviceObject {
	AXE_RTTI_INFO(RenderFence, RenderDeviceObject)
public:
	using Base::Base;

	bool isDone();

protected:
	virtual bool onCheckCompleted() = 0;

	AtomicInt8	_signaled{0};
}; // RenderFence

} // namespace axe