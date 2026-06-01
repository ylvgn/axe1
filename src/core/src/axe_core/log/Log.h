#pragma once

#include <axe_core/base/axe_base.h>
#include <axe_core/string/Fmt.h>
#include <axe_core/string/StringUtil.h>

namespace axe {

#define Log_Level_ENUM_LIST(E) \
	E(Unknown,)	\
	E(Info,)	\
	E(Warning,)	\
	E(Error,)	\
//----
AXE_ENUM_CLASS(Log_Level, u8)


class Log : public NonCopyable {
public:
	using Level = Log_Level;

	static Log* Log::s_get() {
		static Log s;
		return &s;
	}
	
	template<class... Args>
	void write(Level lv, Args&&... args) {
		TempString tmp;
		FmtTo(tmp, AXE_FORWARD(args)...);
		onWrite(lv, tmp);
	}

	void onWrite(Level lv, StrView str);

	void flush();
}; // Log

} //namespace axe