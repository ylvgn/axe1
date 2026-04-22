#pragma once

#include "../base/axe_base.h"
#include "UtfUtil.h"
#include "Fmt.h"

namespace axe {

class StringUtil : public StaticClass {
public:
	static void binToHex(String& out, Span<u8> data);

	static void appendBinToHex(String& out, ByteSpan data);
	
	static bool hasChar(StrView view, char ch) { return StrView::npos != view.find(ch); }

	static Pair<StrView, StrView> splitByChar	(StrView view, StrView seperators);
	static Pair<StrView, StrView> splitByChar	(StrView view, char seperator);
	static Pair<StrView, StrView> getLine		(StrView view);

	static StrView trimChar(StrView view, StrView seperators);

	static const char* findChar			(StrView view, StrView charList, bool ignoreCase);
	static const char* findCharFromEnd	(StrView view, StrView charList, bool ignoreCase);

	static int  ignoreCaseCompare(StrView a, StrView b);
	static int  ignoreCaseCompare(char	  a, char	 b)	{ return tolower(a) - tolower(b); }
	static int  ignoreCaseCompare(wchar_t a, wchar_t b) { return towlower(a) - towlower(b); }

	static constexpr const char* extractFromPrefix(const char* src, const char* prefix);

	static void getTokens(StrView view, Vector<StrView>& outList, char separator);

	static bool tryParse(StrView view, i8 & outValue);
	static bool tryParse(StrView view, i16& outValue);
	static bool tryParse(StrView view, i32& outValue);
	static bool tryParse(StrView view, i64& outValue);

	static bool tryParse(StrView view, u8 & outValue);
	static bool tryParse(StrView view, u16& outValue);
	static bool tryParse(StrView view, u32& outValue);
	static bool tryParse(StrView view, u64& outValue);

	static bool tryParse(StrView view, f32& outValue);
	static bool tryParse(StrView view, f64& outValue);
}; // StringUtil
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(StringUtil);


inline
Pair<StrView, StrView> StringUtil::splitByChar(StrView view, StrView seperators) {
	auto* s = view.begin();
	auto* e = view.end();
	StrView::size_type l0, l1;

	AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
	for (auto* p = s; p < e; ++p) {
		if (hasChar(seperators, *p)) {

			axe_try_safe_assign(l0, p - s);
			auto r0 = StrView(s,   l0);

			axe_try_safe_assign(l1, e - p - 1);
			auto r1 = StrView(p+1, l1);
			return {r0, r1};
		}
	}
	AXE_GCC_WARNING_POP()
	return { view, StrView() };
}

inline
Pair<StrView, StrView> StringUtil::splitByChar(StrView view, char seperator) {
	return splitByChar(view, StrView(seperator));
}

inline
Pair<StrView, StrView> StringUtil::getLine(StrView view) {
	return splitByChar(view, "\n");
}

inline
StrView StringUtil::trimChar(StrView view, StrView charList) {
	auto* p = view.begin();
	auto* e = view.end();
	AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
	for ( ; p < e; ++p) {
		if (!hasChar(charList, *p))
			break;
	}
	AXE_GCC_WARNING_POP()
	StrView::size_type n;
	axe_try_safe_assign(n, e - p);
	return StrView(p, n);
}

constexpr inline
const char* StringUtil::extractFromPrefix(const char* src, const char* prefix) {
	if (!src || !prefix)
		return "";

	AXE_GCC_WARNING_PUSH_AND_DISABLE("-Wunsafe-buffer-usage")
	auto srcLen = charStrlen(src);
	auto prefixLen = charStrlen(prefix);
	if (srcLen < prefixLen)
		return "";

	auto* p = src;
	auto* q = prefix;
	auto* qed = prefix + prefixLen;
	while (q < qed) {
		if (*q != *p) return "";
		++p; ++q;
	}
	return p;
	AXE_GCC_WARNING_POP()
}

} // namespace axe


AXE_INLINE
std::ostream& operator<<(std::ostream& s, const axe::StrView& v) {
	::std::streamsize n;
	::axe::axe_try_safe_assign(n, v.size());
	s.write(v.data(), n);
	return s;
}

template<>
struct fmt::formatter<axe::StrViewW> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const axe::StrViewW& v, fmt::format_context& ctx) {
		axe::TempStringA tmp = axe::UtfUtil::toString(v);
		return fmt::format_to(ctx.out(), "{}", tmp);
	}
};

template<class T, size_t N, bool bEnableOverflow>
struct fmt::formatter<axe::StringT<T, N, bEnableOverflow> > {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const axe::StringT<T, N, bEnableOverflow>& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "{}", v.view());
	}
};