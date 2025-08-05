#include "Quat4.h"

namespace axe {

// explicit specialization to force VisualC check syntax in function body

#ifndef AXE_MATH_USE_SSE
	#error
#else
	template Quat4<float>;
	template Quat4<double>;
#endif


template<> const TypeInfo* TypeOf<Quat4f>() {
	using This = Quat4f;
	using TI_Base = TypeInfoInitNoBase<This>;

	class TI : public TI_Base {
	public:
		TI() : TI_Base("Quat4f") {
			static FieldInfo fi[] = {
				FieldInfo("x", &This::x),
				FieldInfo("y", &This::y),
				FieldInfo("z", &This::z),
				FieldInfo("w", &This::w),
			};
			setFields(fi);
		}
	};
	static TI ti;
	return &ti;
}

} // namespace axe