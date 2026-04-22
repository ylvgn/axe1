#pragma once

#if AXE_RENDER_HAS_DX12

#include <axe_render/RenderCommonBase.h>

#if AXE_OS_WINDOWS
	#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>

	#include <directx/d3d12.h>
	//#include <d3d12.h> // #include <directx/d3d12.h> mismatch #define D3D12_SDK_VERSION, maybe upgrader Windows SDK. or manually dl: https://devblogs.microsoft.com/directx/directx12agility/
	#include <dxgi1_6.h>

	#ifndef D3DX12_NO_STATE_OBJECT_HELPERS
		#define D3DX12_NO_STATE_OBJECT_HELPERS
	#endif
	#include <directx/d3dx12.h>

	#if defined(_DEBUG)
		#include <dxgidebug.h>
	#endif

	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxguid.lib")
	#pragma comment(lib, "dxgi.lib")

	// TODO
	#include <d3dcompiler.h>
	#pragma comment(lib, "D3DCompiler.lib")
#endif // AXE_OS_WINDOWS

#define AXE_DX12_THROWIF_HRESULT_ERROR_SELECT(COUNT) AXE_DX12_THROWIF_HRESULT_ERROR_##COUNT
#define AXE_DX12_THROWIF_HRESULT_ERROR_1(hRESULT)				 do{ auto _axe_tmp_hr = (hRESULT); if (FAILED(_axe_tmp_hr)) { throw ::axe::Error_Win32_HRESULT_String(AXE_LOC, _axe_tmp_hr);				} } while(false)
#define AXE_DX12_THROWIF_HRESULT_ERROR_2(hRESULT, pID3D12Device) do{ auto _axe_tmp_hr = (hRESULT); if (FAILED(_axe_tmp_hr)) { throw ::axe::Error_DX12_HRESULT_String(AXE_LOC, _axe_tmp_hr, pID3D12Device);	} } while(false)
#define AXE_DX12_THROWIF_HRESULT_ERROR(...) AXE_IDENTITY(AXE_CALL(AXE_DX12_THROWIF_HRESULT_ERROR_SELECT, AXE_VA_ARGS_COUNT(__VA_ARGS__)(__VA_ARGS__)))

namespace axe {

#if defined(_DEBUG)
	using DX12_IDXGIDebug							= IDXGIDebug1;
	using DX12_ID3D12Debug							= ID3D12Debug1; // ID3D12Debug6
	using DX12_ID3D12InfoQueue						= ID3D12InfoQueue;
#endif

using DX12_IDXGIAdapter								= IDXGIAdapter4;
using DX12_IDXGIDevice								= IDXGIDevice;
using DX12_IDXGIFactory								= IDXGIFactory7; // use IDXGIFactory4 or newer
using DX12_IDXGIOutput								= IDXGIOutput6;
using DX12_IDXGISwapChain							= IDXGISwapChain4;

using DX12_ID3D12CommandQueue						= ID3D12CommandQueue;
using DX12_ID3D12Device								= ID3D12Device5;
using DX12_ID3D12DeviceRemovedExtendedDataSettings	= ID3D12DeviceRemovedExtendedDataSettings1;
using DX12_ID3D12Fence								= ID3D12Fence1;
using DX12_ID3D12GraphicsCommandList				= ID3D12GraphicsCommandList7; // ID3D12GraphicsCommandList10
using DX12_ID3D12Resource							= ID3D12Resource2;
using DX12_ID3D12DescriptorHeap						= ID3D12DescriptorHeap;

class CommandQueue_DX12;
class Context_DX12;
class Capabilities_DX12;
class Device_DX12;
class DescriptorHandle_DX12;
class Fence_DX12;
class GpuBuffer_DX12;
class Renderer_DX12;
class SwapChain_DX12;


#if 0
#pragma mark ========= DX12Util ============
#endif
class DX12Util : public RenderCommonBase {
public:
	static bool isValid		  (::HRESULT hr);
	static void warningIfError(::HRESULT hr);

	static Renderer_DX12*		renderer();
	static DX12_IDXGIFactory*	dxgiFactory();
	static Device_DX12*			rootDevice();

	static void convert(Rect2f& o, const ::D3D12_RECT& i) {
		using T = decltype(o.x);

		o.x		= T(i.left);
		o.y		= T(i.top);
		o.w		= T(i.right - i.left);
		o.h		= T(i.bottom - i.top);
	}

	static void convert(::D3D12_RECT& o, const Rect2f& i) {
		using T  = decltype(o.left);

		o.left	 = T(i.x);
		o.top	 = T(i.y);
		o.right	 = T(i.xMax());
		o.bottom = T(i.yMax());
	}

	static void convert(i64& o, const ::LUID& i) {
		AXE_STATIC_ASSERT(sizeof(o) >= sizeof(i));

		o = static_cast<i64>(i.HighPart) << 32 | i.LowPart;
	}

	static void convert(::LUID& o, const i64& i) {
		AXE_STATIC_ASSERT(sizeof(o) >= sizeof(i));

		using L = decltype(o.LowPart);
		using H = decltype(o.HighPart);

		o.LowPart  = static_cast<L>(i & 0xFFFFFFFF);
		o.HighPart = static_cast<H>((i >> 32) & 0xFFFFFFFF);
	}

	static Rect2f		toRect2f (const ::D3D12_RECT& i)	{ Rect2f o;			convert(o, i);	return o; }
	static ::D3D12_RECT toD3DRect(const Rect2f& i)			{ ::D3D12_RECT o;	convert(o, i);	return o; }

	static void setDebugName(::ID3D12Object* pObject, StrView name);

	static void setResourceCallstack(::ID3D12Object* pObject);
	static bool getResourceCallstack(Callstack<6>& outCallstack, ::ID3D12Object* pObject);

	static ::D3D12_PRIMITIVE_TOPOLOGY getDxPrimitiveTopology(RenderPrimitiveType t);
	static ::DXGI_FORMAT getDxColorType(ColorType type);
	static ::DXGI_FORMAT getDxDataType(RenderDataType type);

private:
	static bool _checkError(::HRESULT hr) {
		return FAILED(hr); // if got error, return true
	}
};
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(DX12Util);


inline
bool DX12Util::isValid(::HRESULT hr) {
	if (_checkError(hr))
	{
		warningIfError(hr);
		return false;
	}
	return true;
}

inline
void DX12Util::warningIfError(::HRESULT hr) {
	if (_checkError(hr)) {
		AXE_LOG_WARN("HRESULT(0x{:0X}): {}", hr, Win32_HRESULT_String(hr));
	}
}

inline
::D3D12_PRIMITIVE_TOPOLOGY DX12Util::getDxPrimitiveTopology(RenderPrimitiveType t) {
	using SRC = RenderPrimitiveType;
	switch (t) {
		case SRC::Points:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case SRC::Lines:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case SRC::Triangles:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//---
		default:
			AXE_THROW;
	}
}

inline
::DXGI_FORMAT DX12Util::getDxColorType(ColorType type) {
	using SRC = ColorType;
	switch (type) {
//		case SRC::HSBAf: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAf: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case SRC::RGBAh: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case SRC::RGBAb: return DXGI_FORMAT_R8G8B8A8_UNORM;
//		case SRC::RGBb:  
		case SRC::RGb:	 return DXGI_FORMAT_R8G8_UNORM;
		case SRC::Rb:	 return DXGI_FORMAT_R8_UNORM;
//		case SRC::Ab:	 return DXGI_FORMAT_A8_UNORM;
		case SRC::Lb:	 return DXGI_FORMAT_R8_UNORM;
//		case SRC::Af:	 return DXGI_FORMAT_R32_FLOAT;
		case SRC::Lf:	 return DXGI_FORMAT_R32_FLOAT;
		case SRC::LAf:	 return DXGI_FORMAT_R32G32_FLOAT;
		case SRC::LAb:	 return DXGI_FORMAT_R8G8_UNORM;
	//---
		case SRC::BC1:		return DXGI_FORMAT_BC1_UNORM;
		case SRC::BC2:		return DXGI_FORMAT_BC2_UNORM;
		case SRC::BC3:		return DXGI_FORMAT_BC3_UNORM;
		case SRC::BC4:		return DXGI_FORMAT_BC4_UNORM;
		case SRC::BC5:		return DXGI_FORMAT_BC5_UNORM;
		case SRC::BC6h:		return DXGI_FORMAT_BC6H_UF16;
		case SRC::BC7:		return DXGI_FORMAT_BC7_UNORM;
	//---
		default:
			AXE_THROW;
	}
}

inline
::DXGI_FORMAT DX12Util::getDxDataType(RenderDataType type) {
	using SRC = RenderDataType;
	switch (type) {
		case SRC::UInt8:		return DXGI_FORMAT_R8_UNORM; break;
		case SRC::UInt8x2:		return DXGI_FORMAT_R8G8_UNORM; break;
//		case SRC::UInt8x3:		return DXGI_FORMAT_R8G8B8_UNORM; break; //does not support in DX11
		case SRC::UInt8x4:		return DXGI_FORMAT_R8G8B8A8_UNORM; break;
	//--
		case SRC::Float16:		return DXGI_FORMAT_R16_FLOAT; break;
		case SRC::Float16x2:	return DXGI_FORMAT_R16G16_FLOAT; break;
//		case SRC::Float16x3:	return DXGI_FORMAT_R16G16B16_FLOAT; break; //does not support in DX11
		case SRC::Float16x4:	return DXGI_FORMAT_R16G16B16A16_FLOAT; break;
	//---
		case SRC::Float32:		return DXGI_FORMAT_R32_FLOAT; break;
		case SRC::Float32x2:	return DXGI_FORMAT_R32G32_FLOAT; break;
		case SRC::Float32x3:	return DXGI_FORMAT_R32G32B32_FLOAT; break;
		case SRC::Float32x4:	return DXGI_FORMAT_R32G32B32A32_FLOAT; break;
	//---
		default:
			AXE_THROW;
	}
}

#if 0
#pragma mark ========= DX12_HRESULT_String ============
#endif
class DX12_HRESULT_String {
	using This = DX12_HRESULT_String;
	using Util = DX12Util;
public:
	DX12_HRESULT_String(::HRESULT hr, ::ID3D12Device* pDevice);

	StrView		strView() const	{ return _str; }
	operator	StrView() const	{ return strView(); }

	void onFormat(fmt::format_context& ctx) const;
private:
	String_<1024> _str;
}; // DX12_HRESULT_String
AXE_FORMATTER(DX12_HRESULT_String)


#if 0
#pragma mark ========= Error_DX12_HRESULT_String ============
#endif
class Error_DX12_HRESULT_String : public Error {
	using This = Error_DX12_HRESULT_String;
	using Base = Error;
public:
	explicit Error_DX12_HRESULT_String(const SrcLoc& loc, ::HRESULT hr, ::ID3D12Device* pDevice) {
		_loc = loc;
		DX12_HRESULT_String s(hr, pDevice);
		FmtTo(_msg, "HRESULT Error(0x{:0X}): {}", hr, s);
		Base::_assert();
	}
}; // Error_DX12_HRESULT_String
AXE_FORMATTER(Error_DX12_HRESULT_String)

} // namespace axe


template<>
struct fmt::formatter<::LUID> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const ::LUID& v, fmt::format_context& ctx) {
		::axe::i64 combine;
		::axe::DX12Util::convert(combine, v);
		return fmt::format_to(ctx.out(), "{}(HighPart: {}, LowPart: {})", combine, v.HighPart, v.LowPart);
	}
};


constexpr bool operator==(const ::LUID& lhs, const ::LUID rhs) {
	return lhs.LowPart == rhs.LowPart && lhs.HighPart == rhs.HighPart;
}
constexpr bool operator!=(const ::LUID& lhs, const ::LUID rhs) {
	return !(operator==(lhs, rhs));
}

#endif // AXE_RENDER_HAS_DX12