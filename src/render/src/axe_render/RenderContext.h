#pragma once

#include "RenderDeviceObject.h"

namespace axe {

class RenderContext_CreateDesc : public RenderDeviceObject_CreateDesc {
public:
	NativeUIWindow* window = nullptr;
}; // RenderContext_CreateDesc


class RenderContext : public RenderDeviceObject {
	using This = RenderContext;
	using Base = RenderDeviceObject;
public:
	using CreateDesc = RenderContext_CreateDesc;

	virtual ~RenderContext() = default;

	void beginRender();
	void endRender();

	void setFrameBufferSize(Vec2f newSize);
	const Vec2f& frameBufferSize() const { return _frameBufferSize; }

protected:
	RenderContext(CreateDesc& desc) noexcept; // please create from 'RenderDevice::createRenderContext'

	virtual void onBeginRender() {}
	virtual void onEndRender() {}

	virtual void onSetFrameBufferSize(const Vec2f& newSize) {};

	Vec2f _frameBufferSize {0,0};
}; // RenderContext

} // namespace axe