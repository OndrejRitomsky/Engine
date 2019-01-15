#pragma once

#include "config.h"


// @TODO separate enable dissable logging for subsystems 
namespace eng {

	//ENG: 
	//ENG_INFO: 
	//ENG_WARNING:
	//ENG_ERROR:

	void LogSetFunction(void(*LogFn)(const char* text));

#ifdef ENG_LOGGING_ENABLED

	void Log(const char* format, ...);

	void LogInfo(const char* format, ...);

	void LogWarning(const char* format, ...);

	void LogError(const char* format, ...);


#else

	void Log(const char* format, ...) {}

	void LogInfo(const char* format, ...) {}

	void LogWarning(const char* format, ...) {}

	void LogError(const char* format, ...) {}

#endif
}
