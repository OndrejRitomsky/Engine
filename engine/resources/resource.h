#pragma once

#include <core/common/types.h>

namespace eng {
	// @TODO shouldnt be void*
	typedef u64 Resource;

	static const Resource INVALID_RESOURCE_VALUE = 0xFFFFffffFFFFffff;

	enum class ResourceType : u8 {
		NONE,
		TEXTURE,
		SHADER_STAGE,
		SHADER_PROGRAM,
		MESH, // maybe will need template
		MATERIAL,
		MATERIAL_TEMPLATE,
		RENDER_OBJECT
	};

}