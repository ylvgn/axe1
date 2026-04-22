#include "Reflection.h"

namespace axe {

AXE_TYPEOF_PRIMITIVE_IMP(void,	  "void")
AXE_TYPEOF_PRIMITIVE_IMP(bool,	  "bool")

AXE_TYPEOF_PRIMITIVE_IMP(int8_t,  "i8")
AXE_TYPEOF_PRIMITIVE_IMP(int16_t, "i16")
AXE_TYPEOF_PRIMITIVE_IMP(int32_t, "i32")
AXE_TYPEOF_PRIMITIVE_IMP(int64_t, "i64")

AXE_TYPEOF_PRIMITIVE_IMP(uint8_t,  "u8")
AXE_TYPEOF_PRIMITIVE_IMP(uint16_t, "u16")
AXE_TYPEOF_PRIMITIVE_IMP(uint32_t, "u32")
AXE_TYPEOF_PRIMITIVE_IMP(uint64_t, "u64")

AXE_TYPEOF_PRIMITIVE_IMP(float,	      "f32")
AXE_TYPEOF_PRIMITIVE_IMP(double,	  "f64")
AXE_TYPEOF_PRIMITIVE_IMP(long double, "f128")

//AXE_TYPEOF_PRIMITIVE_IMP(char8_t,  "char8") // char8_t: require c++20
AXE_TYPEOF_PRIMITIVE_IMP(char,	   "char8")
AXE_TYPEOF_PRIMITIVE_IMP(char16_t, "char16")
AXE_TYPEOF_PRIMITIVE_IMP(char32_t, "char32")
AXE_TYPEOF_PRIMITIVE_IMP(wchar_t,  "wchar")


void FieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldType->name);
}

const TypeInfo* TypeInfo::s_getType() {
	using This	  = TypeInfo;
	using TI_Base = TypeInfoInitNoBase<This>;

	class TI : public TI_Base {
	public:
		TI() : TI_Base("TypeInfo") {
			#if false // TOOD how to fix ???
			static FieldInfo fi[] = {
				FieldInfo("name",		 &This::name),
				FieldInfo("base",		 &This::base),
				FieldInfo("elementType", &This::elementType),
				FieldInfo("creator",	 &This::creator),
				FieldInfo("dataSize",	 &This::dataSize),
				FieldInfo("isContainer", &This::isContainer),
			};
			setFields(fi);
			#endif
		}
	};
	static TI ti;
	return &ti;
}

void TypeInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, dataSize={}", name, dataSize);
	if (base) {
		fmt::format_to(ctx.out(), ", base=[{}]", *base);
	}
	if (!_fields.empty()) fmt::format_to(ctx.out(), "{}", ", fields={");
	for (auto& f : _fields) {
		fmt::format_to(ctx.out(), " [{}],", f);
	}
	if (!_fields.empty()) fmt::format_to(ctx.out(), " {}", '}');
}

} // namespace axe