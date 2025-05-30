#include "Reflection.h"

namespace axe {

AXE_TYPEOF_SIMPLE_IMP(float,  "float")
AXE_TYPEOF_SIMPLE_IMP(double, "double")

AXE_TYPEOF_SIMPLE_IMP(int8_t,  "int8")
AXE_TYPEOF_SIMPLE_IMP(int16_t, "int16")
AXE_TYPEOF_SIMPLE_IMP(int32_t, "int32")
AXE_TYPEOF_SIMPLE_IMP(int64_t, "int64")

AXE_TYPEOF_SIMPLE_IMP(uint8_t,	"uint8")
AXE_TYPEOF_SIMPLE_IMP(uint16_t, "uint16")
AXE_TYPEOF_SIMPLE_IMP(uint32_t, "uint32")
AXE_TYPEOF_SIMPLE_IMP(uint64_t, "uint64")

AXE_TYPEOF_SIMPLE_IMP(char8_t,	"char8")
AXE_TYPEOF_SIMPLE_IMP(char16_t, "char16")
AXE_TYPEOF_SIMPLE_IMP(char32_t, "char32")
AXE_TYPEOF_SIMPLE_IMP(wchar_t,	"wchar")




void FieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldType->name);
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