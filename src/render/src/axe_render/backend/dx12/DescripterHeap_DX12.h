#pragma once

#if AXE_RENDER_HAS_DX12

namespace axe {

template<class NATIVE_HANDLE>
class DescriptorHandleImpl : public NATIVE_HANDLE {
/*
- Refs:
  - d3dx12_root_signature.h
*/
	using This = DescriptorHandleImpl;
public:
	using NativeHandle = NATIVE_HANDLE;

	using NativeHandle::ptr;
	using SizeType = decltype(ptr);

			 This() = default;
			 This(Null)					  noexcept { ptr = 0; }
    explicit This(const NativeHandle & r) noexcept : NativeHandle(r) {}

	This(const NativeHandle& r, INT offsetScaledByIncrementSize) noexcept {
        initOffsetted(r, offsetScaledByIncrementSize);
    }

	This(const NativeHandle& r
		, INT offsetInDescriptors
		, UINT descriptorIncrementSize) noexcept
	{
        initOffsetted(r, offsetInDescriptors, descriptorIncrementSize);
    }

	explicit operator bool() const { return ptr != SizeType(0); }

	void operator=(const NativeHandle& r)	noexcept { ptr = r.ptr; }
	void operator=(NativeHandle && r)		noexcept { ptr = r.ptr; r.ptr = 0; }

	void offset(INT offsetInDescriptors, UINT descriptorIncrementSize) noexcept {
		ptr = SizeType(intptr_t(ptr)
			+ intptr_t(offsetInDescriptors) * intptr_t(descriptorIncrementSize));
    }

	void offset(intptr_t offsetScaledByIncrementSize) noexcept {
		ptr = SizeType(intptr_t(ptr) + offsetScaledByIncrementSize);
    }

	bool operator == (const NativeHandle& r) const noexcept { return (ptr == r.ptr); }
	bool operator != (const NativeHandle& r) const noexcept { return (ptr != r.ptr); }

	void operator += (intptr_t s) { offset(s); }
	void operator -= (intptr_t s) { offset(s); }

	intptr_t operator+ (intptr_t s) { return intptr_t(ptr) + s; }
	intptr_t operator- (intptr_t s) { return intptr_t(ptr) - s; }

    void initOffsetted(const NativeHandle& r, INT offsetScaledByIncrementSize) noexcept {
        s_initOffsetted(*this, r, offsetScaledByIncrementSize);
    }

    void initOffsetted(const NativeHandle& r
					 , INT offsetInDescriptors
					 , UINT descriptorIncrementSize) noexcept {
        s_initOffsetted(*this, r, offsetInDescriptors, descriptorIncrementSize);
    }

    static inline void s_initOffsetted(NativeHandle& out
								     , const NativeHandle& base
								     , INT offsetScaledByIncrementSize) noexcept
    {
		out.ptr = SizeType(intptr_t(base.ptr) + intptr_t(offsetScaledByIncrementSize));
    }

    static inline void s_initOffsetted(NativeHandle& out
								     , const NativeHandle& base
								     , INT  offsetInDescriptors
								     , UINT descriptorIncrementSize) noexcept
    {
		out.ptr = SizeType(intptr_t(base.ptr) 
			+ intptr_t(offsetInDescriptors) * intptr_t(descriptorIncrementSize));
    }

	decltype(ptr) detach() noexcept { auto o = ptr; ptr = 0; return o; }
}; // DescriptorHandleImpl

using CpuDescriptorHandle_DX12 = DescriptorHandleImpl<::D3D12_CPU_DESCRIPTOR_HANDLE>;
using GpuDescriptorHandle_DX12 = DescriptorHandleImpl<::D3D12_GPU_DESCRIPTOR_HANDLE>;

class DescriptorHandle_DX12 {
public:
	using CpuHandle = CpuDescriptorHandle_DX12;
	using GpuHandle = GpuDescriptorHandle_DX12;

	DescriptorHandle_DX12() = default;
	DescriptorHandle_DX12(const CpuHandle::NativeHandle& h) noexcept : cpu(h) {}
	DescriptorHandle_DX12(const GpuHandle::NativeHandle& h) noexcept : gpu(h) {}

	CpuHandle cpu = {nullptr};
	GpuHandle gpu = {nullptr};

	operator CpuHandle() const { return cpu; }
	operator GpuHandle() const { return gpu; }
}; // DescriptorHandle_DX12

} // namespace axe

#endif // AXE_RENDER_HAS_DX12