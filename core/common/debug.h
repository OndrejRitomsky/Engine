#pragma once

#include "config.h"

#if CORE_PARANOIA_LEVEL > 0

#include <assert.h>

#define ASSERT(value) assert((value));

#define ASSERT2(value, message) assert((value) && (message));

#else

#define ASSERT(value) ((void*)0);

#define ASSERT2(value, message) ((void*)0);

#endif 
