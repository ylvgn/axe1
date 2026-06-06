#pragma once

namespace axe {

struct ImuiApp_CreateDesc : public NativeUIApp_CreateDesc {
	Renderer::CreateDesc	 rendererCreateDesc;
	RenderDevice::CreateDesc renderDeviceCreateDesc;
};

class ImUIApp : public NativeUIApp {
	using This = ImUIApp;
	using Base = NativeUIApp;
public:
	AXE_DOWNCAST_GET_INSTANCE()

	using CreateDesc = ImuiApp_CreateDesc;

	ImUIApp(const CreateDesc& desc = CreateDesc()) noexcept
		: Base(desc)
	{
		_rendererCreateDesc     = desc.rendererCreateDesc;
		_renderDeviceCreateDesc = desc.renderDeviceCreateDesc;
	}

	Renderer*		renderer()		{ return _renderer.get(); }
	RenderDevice*	renderDevice()	{ return _renderDevice.get(); }

protected:
	virtual void onCreate() override;

	Renderer::CreateDesc	 _rendererCreateDesc;
	RenderDevice::CreateDesc _renderDeviceCreateDesc;

private:
	UPtr<Renderer>	   _renderer;
	UPtr<RenderDevice> _renderDevice;
};

} // namespace axe