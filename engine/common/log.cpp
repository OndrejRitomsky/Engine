#include "log.h"


#include "config.h"


#include <core/algorithm/format.h>
#include <core/platform/log.h>

#include <stdarg.h>

static void(*g_LogFn)(const char* text) = core::LogVS;

// @TODO should be thread safe?
namespace eng {

	void LogSetFunction(void(*LogFn)(const char* text)) {
		g_LogFn = LogFn;
	}

#ifdef ENG_LOGGING_ENABLED

	static void LogList(const char* format, va_list list) {
		char buff[1024];
		core::FormatVA(buff, 1024, format, list);
		g_LogFn(buff);
	}

	void Log(const char* format, ...) {
		va_list args;
		va_start(args, format);
		g_LogFn("ENG: ");
		LogList(format, args);
		va_end(args);
	}

	void LogInfo(const char* format, ...) {
		va_list args;
		va_start(args, format);
		g_LogFn("ENG_INFO: ");
		LogList(format, args);
		va_end(args);
	}

	void LogWarning(const char* format, ...) {
		va_list args;
		va_start(args, format);
		g_LogFn("ENG_WARNING: ");
		LogList(format, args);
		va_end(args);
	}

	void LogError(const char* format, ...) {
		va_list args;
		va_start(args, format);
		g_LogFn("ENG_ERROR: ");
		LogList(format, args);
		va_end(args);
	}
#endif

}
