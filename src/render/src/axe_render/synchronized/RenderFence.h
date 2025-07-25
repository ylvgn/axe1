#pragma once

#include "../RenderResource.h"

namespace axe {

class RenderFence_CreateDesc {
public:
	i64 initialFenceValue = 0;
}; // RenderFence_CreateDesc


class RenderFence : public RenderResource {
	AXE_ABSTRACT_CLASS_TYPE(RenderFence, RenderResource)
public:
	using CreateDesc = RenderFence_CreateDesc;

	bool isDone();

friend class Renderer;
friend class RenderDevice;
friend class RenderRequest;
protected:
	void create(CreateDesc& desc); // please create from RenderDevice::createFence
	
	virtual void onCreate(CreateDesc& desc) = 0;

	virtual bool onCheckCompleted() = 0;

	AtomicInt8	_signaled{0};
}; // RenderFence

} // namespace axe