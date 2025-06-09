#pragma once

#include "../base/axe_base.h"

#define AXE_FORMATTER_T(TEMPLATE_ARGS, CLASS) \
	} /* namespace axe */ \
	template<TEMPLATE_ARGS> \
	struct fmt::formatter<axe::CLASS> { \
		static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); } \
		static auto format(const axe::CLASS& v, fmt::format_context& ctx) { \
			v.onFormat(ctx); \
			return ctx.out(); \
		} \
	}; \
	namespace axe { \
//----

#define AXE_FORMATTER(CLASS) AXE_FORMATTER_T(AXE_EMPTY, CLASS)

#define AXE_FORMATTER_ENUM_AS_INT(T) \
	} /* namespace axe */ \
	template<> \
	struct fmt::formatter<axe::T> { \
		static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); } \
		static auto format(const axe::T& v, fmt::format_context& ctx) { \
			return fmt::format_to(ctx.out(), "{}", axe::enumInt(v)); \
		} \
	}; \
	namespace axe { \
//----

template<class T>
struct fmt::formatter<T, typename std::enable_if_t<std::is_enum_v<T>, char> > {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const T& v, fmt::format_context& ctx) {
		using namespace axe;
		return formatter<StrView>::format(StrView_c_str(enumStr(v)), ctx);
	}
};

namespace axe {

template<class STR, class... ARGS> inline
void FmtTo(STR& outStr, ARGS&&... args) {
	AXE_STATIC_ASSERT(sizeof...(args) > 0);
	fmt::format_to(std::back_inserter(outStr), AXE_FORWARD(args)...);
}

template<class... ARGS> inline
TempString Fmt(ARGS&&... args) {
	TempString o;
	FmtTo(o, AXE_FORWARD(args)...);
	return o;
}

} // namespace axe