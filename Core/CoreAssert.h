#pragma once

#include <assert.h>

#define ASSERT(value) assert((value));

#define ASSERT2(value, message) assert((value) && (message));
