#pragma once

#include <Core/Common/Types.h>
#include "Engine/Resources/Resource/Resource.h"

namespace eng {

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


