#include "RenderObject.h"

namespace axe {

RenderObject::RenderObject(CreateDesc& desc) noexcept {
	setDebugName(desc.debugName);
}

} // namespace axe

