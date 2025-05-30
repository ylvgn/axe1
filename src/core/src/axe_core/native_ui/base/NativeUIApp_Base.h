#pragma once

#include <axe_core/app/ConsoleApp.h>

namespace axe {

class NativeUIApp_Base : public ConsoleApp {
	using Base = ConsoleApp;
public:
	struct CreateDesc {
	};

			void run(CreateDesc& desc);
			void update(float dt);
			void quit(int returnCode);
	virtual void willQuit() {}

	void	setFps(int fps);

	int		fps()				const { return _fps; }
	u64		frameCount()		const { return _frameCount; }
	float	targetFrequency()	const { return _targetFrequency; }

protected:
	virtual void onCreate(CreateDesc& desc) {}
	virtual void onUpdate(float dt) {};
	virtual	void onQuit() {}

	constexpr static int	kFastForwardFps			= 60;
	constexpr static float	kFastForwardFrequency	= 1.f / kFastForwardFps;

	u64		_tickCount			= 0;
	u64		_frameCount			= 0;
	float	_deltaTime			= 0;

	int		_fps				= 60;
	float	_targetFrequency	= 1.f / _fps;
	float	_fastForwardMinSec	= _targetFrequency * 20 + kFastForwardFrequency;
	float	_acceptableMaxSec	= _targetFrequency * 0.2f;
}; // NativeUIApp_Base

} // namespace axe