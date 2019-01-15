#include "format.h"


#include <stdio.h>

namespace core {
	u32 FormatVA(char* buffer, u32 bufferSize, const char* format, va_list list) {
		return vsnprintf(buffer, bufferSize, format, list);
	}


}