#include "Object.h"

namespace axe {

template<> const TypeInfo* TypeOf<Object>() {
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
