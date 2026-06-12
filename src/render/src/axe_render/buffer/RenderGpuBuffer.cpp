#include "RenderGpuBuffer.h"

namespace axe {

void RenderGpuBuffer::create(const CreateDesc& desc) {
	AXE_ASSERT(_device != nullptr);
	_desc = desc;
	onCreate(desc);
}

void RenderGpuBuffer::uploadToGpu(ByteSpan data, size_t offset) {
	if (data.size() + offset > _desc.bufferSize) {
		AXE_THROW_ERROR("out of range");
	}
	onUploadToGpu(data, offset);
}

} // namespace axe