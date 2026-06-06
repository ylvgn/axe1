#pragma once

#include "ImUIView.h"

namespace axe {

class ImUIRenderView : public ImUIView {
	using This = ImUIRenderView;
	using Base = ImUIView;
public:
	void createRenderContext(RenderContext::CreateDesc& desc);

	RenderContext* renderContext() { return _renderContext.get(); }

	void render();

protected:
	virtual void onSizeChanged(const Vec2f& size) override;

private:
	UPtr<RenderContext> _renderContext;
};

} // namespace axe