#pragma once

#include <Core/Common/Types.h>

#include "Engine/Resources/Resource/Resource.h"

namespace eng {

	struct RenderObjectDescription {
		h64 shaderProgram;
		h64 material;
		h64 hash;
	};

	struct RenderObject {
		Resource shaderProgram;
		Resource material;
		Resource handle;
	};
}


