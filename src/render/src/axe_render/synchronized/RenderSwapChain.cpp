#include "RenderSwapChain.h"

namespace axe {

const TypeInfo* RenderSwapChain::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			name = "RenderSwapChain";
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

} // namespace axe 