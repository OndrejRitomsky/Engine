#pragma once

#include "../common/types.h"

// @TODO ?? Leaking, can this be even fixed?
#include <stdarg.h>

namespace core {
	u32 FormatVA(char* buffer, u32 bufferSize, const char* format, va_list list);
}