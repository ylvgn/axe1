#include "RenderGpuBuffer.h"

namespace axe {

const TypeInfo* RenderGpuBuffer::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderGpuBuffer";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

void RenderGpuBuffer::create(CreateDesc& desc) {
	AXE_ASSERT(_device != nullptr);
	_desc = desc;
	onCreate(desc);
}

void RenderGpuBuffer::uploadToGpu(ByteSpan data, size_t offset) {
	if (data.size() + offset > _desc.bufferSize) {
		throw AXE_ERROR("out of range");
	}
	onUploadToGpu(data, offset);
}

} // namespace axe