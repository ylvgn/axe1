#pragma once

#include <axe_core/string/UtfUtil.h>
#include <axe_core/string/StringUtil.h>

namespace axe {

#define CommandLine_ParamAssignment_ENUM_LIST(E) \
	E(Space,) \
	E(Equals,) \
	E(ColonEquals,) \
//----
AXE_ENUM_DECLARE(CommandLine_ParamAssignment, u8)

struct CommandLine_Param {
	using This = CommandLine_Param;
	using Assignment = CommandLine_ParamAssignment;

	CommandLine_Param() noexcept = delete;

	CommandLine_Param(StrView  name) noexcept : opName(name) {}
	CommandLine_Param(StrViewW name) noexcept { UtfUtil::convert(opName, name); }

	CommandLine_Param(StrView name, i8&  v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, i16& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, i32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, i64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	CommandLine_Param(StrView name, u8&  v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, u16& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, u32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, u64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	CommandLine_Param(StrView name, f32& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }
	CommandLine_Param(StrView name, f64& v) noexcept : opName(name) { StringUtil::tryParse(opValue, v); }

	CommandLine_Param(StrView name, StrView  v) noexcept : opName(name) { opValue.assign(v.data(), v.size()); }
	CommandLine_Param(StrView name, StrViewW v) noexcept : opName(name) { UtfUtil::convert(opValue, v); }

	void opNameW(StringW& out)				const { UtfUtil::convert(out, opName);  }
	void opValueW(StringW& out)				const { UtfUtil::convert(out, opValue); }
	void opAssignmentW(StringW& out)		const { UtfUtil::convert(out, assignment()); }

	void opNameW (TempStringW& out)			const { UtfUtil::convert(out, opName);  }
	void opValueW(TempStringW& out)			const { UtfUtil::convert(out, opValue); }
	void opAssignmentW(TempStringW& out)	const { UtfUtil::convert(out, assignment()); }

	bool empty() const { return opName.empty(); }

	const char* name()			const { return opName.c_str();  }
	const char* value()			const { return opValue.c_str(); }
	const char* assignment()	const;

	String opName;
	String opValue;
	Assignment opAssignment = Assignment::Space;
};

struct CommandLine {
	using Param = CommandLine_Param;

	CommandLine() = delete;

	static void runShell(StrView execFileName, const Span<Param> params = Span<Param>(), StrView workingDir = StrView());
}; // CommandLine

} // namespace axe