#pragma once

#include <core/common/types.h>
#include "resource.h"

namespace eng {

	// @TODO unused?
	struct ShaderProgramDescription {
		h64 vertexStage;
		h64 fragmentStage;
		h64 hash;
	};

	struct ShaderProgram {
		Resource vertexStage;
		Resource fragmentStage;
		//Resource handle;
	};
}


