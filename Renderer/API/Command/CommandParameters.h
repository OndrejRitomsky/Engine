#pragma once

#include <Core/Types.h>

namespace render { 
	enum ClearFlags : u8 {
		COLOR = 1,
		DEPTH = 2,
		STENCIL = 4
	};
}