#pragma once

#include <Core/Types.h>

#include "Renderer/API/Resources/Resource.h"

namespace render {
	//---------------------------------------------------------------------------
	struct ShaderStage {
		u32 dataSize;
		char* data;
	};

	//---------------------------------------------------------------------------
	struct Shader : Resource {
		ShaderStage vertexStage;
		ShaderStage fragmentStage;
	};
}