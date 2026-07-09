#pragma once

#include "../RenderDeviceObject.h"

namespace axe {

class RenderSwapChain : public RenderDeviceObject {
	AXE_RTTI_INFO(RenderSwapChain, RenderDeviceObject)
public:
	using Base::Base;

	constexpr static const size_t kFrameBufferCount = 2;

	u32 curImageIdx() const { return _curImageIdx; }

	const auto& frameBufferSize() const { return _frameBufferSize; }

	static const constexpr Vec2i  kMinFrameSize{8, 8};
	
protected:
	virtual void OnResizeOrMove(const Vec2i& newSize) = 0;

	Vec2i			_frameBufferSize { 0,0 }; // TODO, may no need atm
	u32				_curImageIdx = 0;
	RenderContext*	_renderContext = nullptr;
}; // RenderSwapChain

} // namespace axe