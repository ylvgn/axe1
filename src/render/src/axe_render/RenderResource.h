#pragma once

#include "RenderDeviceObject.h"

namespace axe {

#define RenderResourceType_ENUM_LIST(E) \
	E(Unknown, )                        \
	E(RenderGpuBuffer, )                \
	E(Texture, )                        \
	E(Shader, )                         \
	E(Material, )                       \
	E(_End, )                           \
//----
AXE_ENUM_CLASS(RenderResourceType, u8);


class RenderResource : public RenderDeviceObject {
	AXE_CLASS_TYPE(RenderResource, RenderDeviceObject)
public:
	using Base::Base;

	static constexpr RenderResourceType kRscType = RenderResourceType::Unknown;

	virtual RenderResourceType rscType() { return kRscType; }

	virtual void onRefCountZero() override {
		AXE_TODO("send resource to renderer to clean up in render thread");
		axe_delete(this);
	}
}; // RenderResource

#define axeRenderResources_InterfaceFunctions(T)                            \
	static constexpr RenderResourceType kRscType = RenderResourceType::##T; \
                                                                            \
	virtual RenderResourceType rscType()                                    \
	{                                                                       \
		return kRscType;                                                    \
	}                                                                       \
//----

} // namespace axe