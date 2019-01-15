#pragma once


#include <core/common/types.h>
#include <core/common/debug.h>


namespace render {
// -------------------------- resource ----------------------------------------
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



	static inline void InitResource(Resource* resource, u32 handle, ResourceType type) {
		u64 t = static_cast<u64>(type);
		resource->handle = t << 32 | handle;
	}

	static inline ResourceType ResourceGetType(const Resource* resource) {
		return static_cast<ResourceType>(resource->handle >> 32);
	}



// -------------------------- buffer ------------------------------------------
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
		F32 = 1,
		V2 = 2,
		M4 = 16,
	};

	struct VertexAttribute {
		bool normalize;
		VertexAttributeType type;

		u32 index;
		u32 offset;
		u32 stride;
		u32 divisor;

		u64 vertexBufferHandle; // from which buffer this attribute comes
	};

	// Vertex buffers referenced, have to be already created!!! // @TODO ??
	// @TODO what name directX uses for attributes (to have common name for opengl/directx ...)
	struct VertexDescription : Resource {
		static const u32 MAX_ATTRIBUTES = 8;
		bool hasIndexBuffer;

		VertexAttribute attributes[MAX_ATTRIBUTES];

		u32 attributesCount;
		u64 indexBufferHandle;
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

	inline void AddVertexAttribute(VertexDescription* rvi, u64 handle, u32 index, u32 offset, u32 stride, u32 divisor, VertexAttributeType type);


	void AddVertexAttribute(VertexDescription* rvi, u64 handle, u32 index, u32 offset, u32 stride, u32 divisor, VertexAttributeType type) {
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


// -------------------------- shader ------------------------------------------
	struct ShaderStage {
		//u32 dataSize;
		const char* data;
	};

	struct Shader : Resource {
		ShaderStage vertexStage;
		ShaderStage fragmentStage;
	};

// -------------------------- texture ------------------------------------------


// Only 2D for now and only generate or not mipmaps

	enum class TextureFilterType : u8 {
		Linear,
		Nearest,
		LinearMipMapLinear
	};

	enum class TextureWrapType : u8 {
		Repeat
	};

	enum class TextureFormat : u8 {
		RGB,
		RGBA
	};

	struct Texture : Resource {
		TextureFilterType minFilter;
		TextureFilterType maxFilter;

		TextureWrapType wrapS;
		TextureWrapType wrapT;

		TextureFormat format;

		bool mipmaps;

		u32 width;
		u32 height;

		void* data;
	};
}