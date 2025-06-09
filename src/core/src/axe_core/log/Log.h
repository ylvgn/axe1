#pragma once

#include <axe_core/base/axe_base.h>
#include <axe_core/string/Fmt.h>
#include <axe_core/string/StringUtil.h>

#define AXE_LOG(...)		do{ axe::g_log.write(axe::Log::Level::Info,    __VA_ARGS__); } while(false)
#define AXE_LOG_WARN(...)	do{ axe::g_log.write(axe::Log::Level::Warning, __VA_ARGS__); } while(false)
#define AXE_LOG_ERROR(...)	do{ axe::g_log.write(axe::Log::Level::Error,   __VA_ARGS__); } while(false)
#define AXE_FLUSH_LOG()		do{ axe::Log::flush(); } while(false)

#define AXE_WARN_ONCE(...)	do{ axe::g_log.writeOnce(axe::Log::Level::Warning, __VA_ARGS__); } while(false)

#define AXE_LOG_FUNC_NAME() AXE_LOG("FUNC {}", AXE_FUNC_FULLNAME_SZ)

#define AXE_TODO(...)		do{ axe::g_log._internal_TODO(AXE_LOC, __VA_ARGS__); } while(false)

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

	template<class... Args>
	void write(Level lv, Args&&... args) {
		TempString tmp;
		FmtTo(tmp, AXE_FORWARD(args)...);
		onWrite(lv, tmp);
	}

	template<class... Args>
	void writeOnce(Level lv, Args&&... args) {
		AXE_RUN_ONCE(write(lv, AXE_FORWARD(args)...));
	}

	template <class... Args>
	void _internal_TODO(const SrcLoc& srcLoc, Args&&... args) { // please use AXE_TODO
		AXE_RUN_ONCE( \
			TempString tmp = "[TODO] "; \
			FmtTo(tmp, AXE_FORWARD(args)...); \
			FmtTo(tmp, "\n  - [{}]\n", srcLoc); \
			onWrite(Level::Warning, tmp) \
		);
	}

	void onWrite(Level lv, StrView str);

	static void flush();
}; // Log

extern Log g_log;

} //namespace axe