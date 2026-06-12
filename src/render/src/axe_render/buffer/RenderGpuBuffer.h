#pragma once

#include "../RenderResource.h"

namespace axe {

class RenderGpuBuffer_CreateDesc {
public:
	using Type = RenderGpuBufferType;

	Type	type		= Type::None;
	size_t	stride		= 16;
	size_t	bufferSize	= 0;
}; // RenderGpuBuffer_CreateDesc


class RenderGpuBuffer : public RenderResource {
	AXE_RTTI_INFO(RenderGpuBuffer, RenderResource)
public:
	using Type		 = RenderGpuBufferType;
	using CreateDesc = RenderGpuBuffer_CreateDesc;

	using Base::Base;

	axeRenderResources_InterfaceFunctions(RenderGpuBuffer)

	void create(const CreateDesc& desc); // please create from RenderDevice::createGpuBuffer

	size_t	bufferSize() const { return _desc.bufferSize; }
	void	uploadToGpu(ByteSpan data, size_t offset = 0);

	const CreateDesc& desc() const { return _desc; }

protected:
	virtual void onCreate(const CreateDesc& desc) = 0;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) = 0;

	CreateDesc _desc;
}; // RenderGpuBuffer

} // namespace axe