#pragma once

#include <axe_core/base/axe_base.h>
#include <axe_core/string/Fmt.h>
#include <axe_core/string/StringUtil.h>

#define AXE_LOG(...)       do{ axe::g_log.write(axe::Log::Level::Info,    __VA_ARGS__); } while(false)
#define AXE_LOG_WARN(...)  do{ axe::g_log.write(axe::Log::Level::Warning, __VA_ARGS__); } while(false)
#define AXE_LOG_ERROR(...) do{ axe::g_log.write(axe::Log::Level::Error,   __VA_ARGS__); } while(false)

#define AXE_LOG_WARN_ONCE(...) do{ AXE_RUN_ONCE( AXE_LOG_WARN(__VA_ARGS__) ); } while(false)

namespace axe {

#define Log_Level_ENUM_LIST(E) \
	E(Unknown,)	\
	E(Info,)	\
	E(Warning,)	\
	E(Error,)	\
//----
AXE_ENUM_CLASS(Log_Level, u8)
// AXE_ENUM_STR(Log_Level)

class Log : public NonCopyable {
public:
	using Level = Log_Level;

	template<class... Args>
	void write(Level lv, Args&&... args) {
		TempString tmp;
		FmtTo(tmp, AXE_FORWARD(args)...);
		onWrite(lv, tmp);
	}

	void onWrite(Level lv, StrView str);
};

extern Log g_log;

} //namespace axe