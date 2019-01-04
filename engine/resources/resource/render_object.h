#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {

	struct RenderObjectDescription {
		h64 mesh;
		h64 material;
		h64 hash;
	};

	struct RenderObject {
		Resource mesh;
		Resource material;
		//Resource handle;
	};
}


