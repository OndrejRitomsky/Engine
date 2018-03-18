#pragma once

#include <Core/Types.h>
#include <Core/CoreAssert.h>

#include "Renderer/API/Resources/Resource.h"

namespace render {	
	enum class BufferType : u8 {
		STATIC,
		DYNAMIC_UPDATE,
		DYNAMIC_SIZE_AND_UPDATE,
	};

	enum class IndexType {
		U16,
		U32
	};

	enum class VertexAttributeType {
		F32,
		V2,
		M4
	};

	struct VertexAttribute {
		bool normalize;
		VertexAttributeType type;

		u32 vertexBufferHandle; // from which buffer this attribute comes
		u32 index;
		u32 offset;
		u32 stride;
		u32 divisor;
	};
	
	// Vertex buffers referenced, have to be already created!!! // @TODO ??
	// @TODO what name directX uses for attributes (to have common name for opengl/directx ...)
	struct VertexDescription : Resource {
		static const u32 MAX_ATTRIBUTES = 8;

		VertexAttribute attributes[MAX_ATTRIBUTES];
		
		u32 attributesCount;
		u32 indexBufferHandle;
	};

	struct IndexBuffer : Resource {
		BufferType bufferType;
		IndexType indexType;
		u32 dataSize;
		const char* data;
	};

	struct VertexBuffer : Resource {
		BufferType bufferType;
		u32 dataSize;
		const char* data;
	};

	inline void AddVertexAttribute(VertexDescription* rvi, u32 handle, u32 index, u32 offset, u32 stride, u32 divisor, VertexAttributeType type);

	//-------------------------------------------------------------------------
	void AddVertexAttribute(VertexDescription* rvi, u32 handle, u32 index, u32 offset, u32 stride, u32 divisor, VertexAttributeType type) {
		ASSERT(rvi->attributesCount + 1 < VertexDescription::MAX_ATTRIBUTES);
		if (rvi->attributesCount + 1 >= VertexDescription::MAX_ATTRIBUTES)
			return;

		u32 i = rvi->attributesCount;

		rvi->attributes[i].vertexBufferHandle = handle;
		rvi->attributes[i].index = index;
		rvi->attributes[i].offset = offset;
		rvi->attributes[i].stride = stride;
		rvi->attributes[i].divisor = divisor;
		rvi->attributes[i].type = type;
		rvi->attributes[i].normalize = false;

		++rvi->attributesCount;
	}
}