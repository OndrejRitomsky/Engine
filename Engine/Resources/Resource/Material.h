#pragma once

#include <Core/Common/Types.h>

#include "Engine/Resources/Resource/Resource.h"

namespace eng {

	struct Texture;
	struct ShaderProgramResource;

	struct MaterialDescription {
		u32 texturesCount;
		h64* textures;
		h64 shaderProgram;
		h64 templateHash;
		h64 hash;
	};

	struct Material {
		u32 texturesCount;
		Resource* textures;
		Resource shaderProgram;
		Resource templateHash;
		Resource handle;
	};
}


