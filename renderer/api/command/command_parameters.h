#pragma once

#include <core/common/types.h>

namespace render { 
	enum ClearFlags : u8 {
		COLOR = 1,
		DEPTH = 2,
		STENCIL = 4
	};
}