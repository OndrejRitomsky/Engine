#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {
	
	struct ShaderStageDescription {
		char* data;
		u32 dataSize;
		h64 hash;
		// shaderstagetype
	};

	struct ShaderStage {
		char* data;
	//	Resource handle;
	};
}


