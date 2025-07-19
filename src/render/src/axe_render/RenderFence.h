#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderFence_CreateDesc : public RenderDeviceObject_CreateDesc {
public:
	i64 initialFenceValue = 0;
}; // RenderFence_CreateDesc


class RenderFence : public RenderObject {
	using This = RenderFence;
	using Base = RenderObject;
public:
	using CreateDesc = RenderFence_CreateDesc;

	bool isDone();

friend class Renderer;
friend class RenderDevice;
friend class RenderRequest;
protected:
	RenderFence(CreateDesc& desc) noexcept; // please create from RenderDevice::createFence

	virtual bool onCheckCompleted() = 0;

	AtomicInt8	_signaled{0};
}; // RenderFence

} // namespace axe