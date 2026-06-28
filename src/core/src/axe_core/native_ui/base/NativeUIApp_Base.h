#pragma once
#include <axe_core/app/AppBase.h>

struct NativeUIApp_CreateDesc {
	bool peekMessage = false;
};

namespace axe {

class NativeUIApp_Base : public AppBase {
	AXE_RTTI_INFO(NativeUIApp_Base, AppBase)
public:
	AXE_DOWNCAST_GET_INSTANCE()

	using CreateDesc = NativeUIApp_CreateDesc;

	NativeUIApp_Base(const CreateDesc& desc);

	virtual void	quit		(int returnCode) = 0;
	
	void update(float dt);

	void	setFps(int fps);

	int		fps()				const { return _fps; }
	u64		frameCount()		const { return _frameCount; }
	float	targetFrequency()	const { return _targetFrequency; }

protected:
	virtual void onUpdate(float dt) {}
	virtual void onPeekMessage() {}

	constexpr static int	kFastForwardFps			= 60;
	constexpr static float	kFastForwardFrequency	= 1.f / kFastForwardFps;

	CreateDesc	_desc;
	
	u64		_tickCount			= 0;
	u64		_frameCount			= 0;
	float	_deltaTime			= 0;

	int		_fps				= 60;
	float	_targetFrequency	= 1.f / float(_fps);
	float	_fastForwardMinSec	= _targetFrequency * 20 + kFastForwardFrequency;
	float	_acceptableMaxSec	= _targetFrequency * 0.2f;
	
	int			_returnCode = 0;
}; // NativeUIApp_Base

} // namespace axe