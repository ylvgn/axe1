#include "Reflection.h"

namespace axe {

void FieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldType->name);
}

void TypeInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, dataSize={}", name, dataSize);
	if (base) {
		fmt::format_to(ctx.out(), ", base=[{}]", *base);
	}
	if (!ownFields.empty()) fmt::format_to(ctx.out(), "{}", ", ownFields={");
	for (auto& f : ownFields) {
		fmt::format_to(ctx.out(), " [{}],", *f);
	}
	if (!ownFields.empty()) fmt::format_to(ctx.out(), " {}", '}');
}

} // namespace axe