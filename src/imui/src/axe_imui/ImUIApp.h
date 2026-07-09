#pragma once

namespace axe {

struct ImuiApp_CreateDesc : public NativeUIApp_CreateDesc {
	RenderDevice::CreateDesc renderSystemDesc;
}; // ImuiApp_CreateDesc

class ImUIApp : public NativeUIApp {
	AXE_RTTI_INFO(ImUIApp, NativeUIApp)
public:
	AXE_DOWNCAST_GET_INSTANCE()

	using CreateDesc = ImuiApp_CreateDesc;

	ImUIApp(const CreateDesc& desc = CreateDesc()) noexcept
		: Base(desc)
	{
		_renderSystemDesc = desc.renderSystemDesc;
	}

	Renderer*		renderer()		{ return _renderer.get(); }
	RenderDevice*	renderDevice()	{ return _renderDevice.get(); }

protected:
	virtual void onCreate() override;

	RenderDevice::CreateDesc _renderSystemDesc;

private:
	UPtr<Renderer>	   _renderer;
	UPtr<RenderDevice> _renderDevice;
}; // ImUIApp

} // namespace axe