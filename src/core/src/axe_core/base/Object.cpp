#include "Object.h"

namespace axe {

const TypeInfo* Object::s_getType() {
	class TI : public TypeInfo {
	public:
		TI() {
			name = "Object";
		}
	};
	static TI ti;
	return &ti;
}

} // namespace
