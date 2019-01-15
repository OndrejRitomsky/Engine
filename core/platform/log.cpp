#include "log.h"

#include <stdio.h>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>


namespace core {

	void LogVS(const char* text) {
		OutputDebugStringA(text);
	}

	void LogStd(const char* text) {
		printf("%s", text);
	}
}