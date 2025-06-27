#pragma once

#include "RenderObject.h"

namespace axe {

class RenderDeviceObject_CreateDesc : public RenderObject_CreateDesc {
public:
	RenderDevice* device = nullptr;
}; // RenderDeviceObject_CreateDesc


class RenderDeviceObject : public RenderObject {
	using This = RenderDeviceObject;
	using Base = RenderObject;
public:
	using CreateDesc = RenderDeviceObject_CreateDesc;

	RenderDevice* device() const { return _device; }

protected:
	RenderDeviceObject(CreateDesc& desc) noexcept;

	RenderDevice* _device = nullptr;
}; // RenderDeviceObject

} // namespace axe