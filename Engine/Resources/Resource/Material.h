#pragma once

#include <core/common/types.h>

#include "resource.h"

namespace eng {

	struct Texture;
	struct ShaderProgramResource;

	struct MaterialDescription {
		u32 texturesCount;
		h64* textures;
		h64 shaderProgram;
		h64 materialTemplate;
		h64 hash;
	};

	struct Material {
		u32 texturesCount;
		Resource* textures;
		Resource shaderProgram;
		Resource materialTemplate;
		//Resource handle;
	};
}


