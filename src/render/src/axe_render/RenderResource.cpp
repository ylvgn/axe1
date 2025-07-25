#include "RenderResource.h"
#include "Renderer.h"

namespace axe {

const TypeInfo* RenderResource::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			AXE_TODO("");
		}
	};
	static TI ti;
	return &ti;
}

} // namespace axe

