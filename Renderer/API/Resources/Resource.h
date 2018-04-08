#pragma once

#include <Core/Common/Types.h>

namespace render {
	enum class ResourceType : u8 {
		SHADER,
		TEXTURE,
		VERTEX_BUFFER,
		VERTEX_DESCRIPTION,
		INDEX_BUFFER
	};

	struct Resource {
		u64 handle; // :8 :32
	};


	static inline void InitResource(Resource* resource, u32 handle, ResourceType type);



	//---------------------------------------------------------------------------
	static inline void InitResource(Resource* resource, u32 handle, ResourceType type) {
		u64 t = static_cast<u64>(type);
		resource->handle = t << 32 | handle;
	}

	//---------------------------------------------------------------------------
	static inline ResourceType ResourceGetType(const Resource* resource) {
		return static_cast<ResourceType>(resource->handle >> 32);
	}
};


