#pragma once

#include <Core/Types.h>

namespace render {
	enum class ResourceType : u8 {
		SHADER,
		TEXTURE,
		VERTEX_BUFFER,
		VERTEX_DESCRIPTION,
		INDEX_BUFFER
	};

	struct Resource {
		ResourceType resourceType;
		u32  handle;
	};

	static inline void InitResource(Resource* resource, u32 handle, ResourceType type);



	//---------------------------------------------------------------------------
	static inline void InitResource(Resource* resource, u32 handle, ResourceType type) {
		resource->handle = handle;
		resource->resourceType = type;
	}
};


