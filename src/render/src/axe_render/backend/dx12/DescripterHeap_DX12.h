#pragma once

#if AXE_RENDER_HAS_DX12

namespace axe {

class DescriptorHandle_DX12 {
public:
	using NativeCpuHandle = ::CD3DX12_CPU_DESCRIPTOR_HANDLE;
	using NativeGpuHandle = ::CD3DX12_GPU_DESCRIPTOR_HANDLE;

	NativeCpuHandle cpu = {};
	NativeGpuHandle gpu = {};

	operator NativeCpuHandle() const { return cpu; }
	operator NativeGpuHandle() const { return gpu; }
}; // DescriptorHandle_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12