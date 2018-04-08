#pragma once

#include <Core/Common/Types.h>

#include "Engine/Resources/Resource/Resource.h"

namespace eng {
	
	struct ShaderStageDescription {
		char* data;
		h64 hash;
		// shaderstagetype
	};

	struct ShaderStage {
		char* data;
	//	Resource handle;
	};
}


