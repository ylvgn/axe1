#pragma once

#include "RenderDeviceObject.h"

namespace axe {

#define RenderResourceType_ENUM_LIST(E) \
	E(Unknown, = 0)                   \
	E(RenderGpuBuffer, )              \
	E(Texture, )                      \
	E(Shader, )                       \
	E(Material, )                     \
	E(_End, )                         \
//----
AXE_ENUM_CLASS(RenderResourceType, u8);

class RenderResource_CreateDesc : public RenderDeviceObject_CreateDesc {
public:

}; // RenderResource_CreateDesc

class RenderResource : public RenderDeviceObject {
	using This = RenderResource;
	using Base = RenderDeviceObject;
public:
	using CreateDesc = RenderResource_CreateDesc;

	static constexpr RenderResourceType kRscType = RenderResourceType::Unknown;

	virtual RenderResourceType rscType() { return kRscType; }

protected:
	RenderResource(CreateDesc& desc) noexcept;

	virtual void onRefCountZero() override {
		AXE_TODO("send resource to renderer to clean up in render thread");
		axe_delete(this);
	}

}; // RenderResource

} // namespace axe