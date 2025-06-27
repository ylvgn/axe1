#if AXE_RENDER_HAS_DX12

#include "Render_DX12_Common.h"
#include "Renderer_DX12.h"
#include "RenderDevice_DX12.h"

#include <axe_core/native_ui/win32/NativeUI_Win32_Common.h>
#include <axe_render/RenderDeviceObject.h>

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
		ComPtr<::ID3D12InfoQueue> pInfo;
		hr = pDevice->QueryInterface(pInfo.ptrForInit());
		if (Util::isValid(hr) && pInfo) {
			_str += "Validation Layer: \n";
			for (UINT64 i = 0; i < pInfo->GetNumStoredMessages(); ++i)
			{
				size_t messageLength = 0;
				pInfo->GetMessage(0, nullptr, &messageLength);
				::D3D12_MESSAGE* pMessage = (::D3D12_MESSAGE*)malloc(messageLength);
				pInfo->GetMessage(0, pMessage, &messageLength);
				_str += StrView(pMessage->pDescription);
				_str += "\n";
				free(pMessage);
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
	return static_cast<Renderer_DX12*>(Renderer::s_instance());
}

RenderDevice_DX12* DX12Util::renderDevice() {
	return DX12Util::renderer()->findDevice();
}

DX12_ID3D12Device* DX12Util::d3dDevice() {
	auto* p = DX12Util::renderDevice();
	return p ? p->d3dDevice() : nullptr;
}

void DX12Util::setDebugName(::ID3D12Object* pObject, StrView name) {
	if (pObject) {
		AXE_DX12_THROWIF_HRESULT_ERROR(pObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<::UINT>(name.size()), name.data()));
	}
}

} // namespace axe

#endif // AXE_RENDER_HAS_DX12

