#pragma once

#include <Core/Common/Types.h>

namespace eng {
	typedef void* Resource;

	enum class ResourceType : u8 {
		NONE,
		TEXTURE,
		SHADER_STAGE,
		SHADER_PROGRAM,
		MATERIAL,
		MATERIAL_TEMPLATE,
		MESH, // maybe will need template
		RENDER_OBJECT
	};
}