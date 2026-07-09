#if AXE_RENDER_HAS_DX12

#if AXE_OS_WINDOWS
	#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>
#endif

#include "Common_DX12.h"
#include "Renderer_DX12.h"
#include "Device_DX12.h"

namespace axe {

DX12_HRESULT_String::DX12_HRESULT_String(::HRESULT errorCode, ID3D12Device* pDevice) {
	::HRESULT hr;
	::LPWSTR tmp;

	if (0 != FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						   NULL,
						   errorCode,
						   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						   (::LPTSTR)&tmp,
						   0,
						   NULL))
	{
		if (tmp)
		{
			UtfUtil::convert(_str, tmp);
		}
		::LocalFree(tmp);
	}

	if (errorCode == DXGI_ERROR_DEVICE_REMOVED && pDevice)
	{
		ComPtr<DX12_ID3D12InfoQueue> pInfo;
		hr = pDevice->QueryInterface(pInfo.ptrForInit());
		if (Util::isValid(hr) && pInfo) {
			_str += "Validation Layer: \n";
			for (::UINT64 i = 0; i < pInfo->GetNumStoredMessages(); ++i)
			{
				size_t messageLength = 0;
				pInfo->GetMessage(i, nullptr, &messageLength);

				::D3D12_MESSAGE* pMessage = (::D3D12_MESSAGE*)malloc(messageLength);
				pInfo->GetMessage(i, pMessage, &messageLength);

				if (pMessage->pDescription)
					_str += StrView(pMessage->pDescription);
				_str += "\n";
				::free(pMessage);
			}
		}

		hr = pDevice->GetDeviceRemovedReason();
		auto DRED = DX12_HRESULT_String(hr, nullptr);
		_str += "\nDRED: ";
		_str += DRED._str;
	}
}

void DX12_HRESULT_String::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "{}", _str);
}

Renderer_DX12* DX12Util::renderer() {
	return Renderer_DX12::s_instance();
}

DX12_IDXGIFactory* DX12Util::dxgiFactory() {
	return renderer()->dxgiFactory();
}

Device_DX12* DX12Util::rootDevice() {
	return static_cast<Device_DX12*>(Renderer::s_rootDevice());
}

void DX12Util::setDebugName(::ID3D12Object* pObject, StrView name) {
	if (pObject) {
		AXE_DX12_THROWIF_HRESULT_ERROR(
			pObject->SetPrivateData(WKPDID_D3DDebugObjectName
									, static_cast<::UINT>(name.size())
									, name.data()) // The data(name) stored in the device internal.
		);
	}
}

static constexpr GUID kResourceCallstackGUID = {
	0xe8241f90,
	0xff0a,
	0x4dd4,
	{ 0xaa, 0xf5, 0xb4, 0x53, 0xe1, 0x91, 0x96, 0x5e }
};
void DX12Util::setResourceCallstack(::ID3D12Object* pObject) {
	AXE_ASSERT(pObject != nullptr);

	Callstack<6> callstack;
	callstack.Trace(1);
	pObject->SetPrivateData(kResourceCallstackGUID, sizeof(callstack), &callstack);
}

bool DX12Util::getResourceCallstack(Callstack<6>& outCallstack, ::ID3D12Object* pObject) {
	AXE_ASSERT(pObject != nullptr);

	auto size = static_cast<::UINT>(sizeof(outCallstack));
	return SUCCEEDED((pObject->GetPrivateData(kResourceCallstackGUID, &size, &outCallstack)));
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12

