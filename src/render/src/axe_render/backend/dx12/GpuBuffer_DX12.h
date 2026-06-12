#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/buffer/RenderGpuBuffer.h>
#include "Common_DX12.h"

namespace axe {

class GpuBuffer_DX12 : public RenderGpuBuffer {
	AXE_RTTI_INFO(GpuBuffer_DX12, RenderGpuBuffer)
public:
	//DX12_ID3DBuffer* d3dBuf() { return _d3dBuf; }

	using Base::Base;

protected:
	virtual void onCreate(const CreateDesc& desc) final;
	virtual void onUploadToGpu(ByteSpan data, size_t offset) final;
private:
	//ComPtr<DX12_ID3DBuffer> _d3dBuf;

}; // GpuBuffer_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12