#include "render_resource_context.h"

#include <core/allocator/allocators.h>
#include <core/algorithm/cstring.h>

#include "render_context_stream.h"
#include "render_resources.h"

#include "internal/commands.h"


namespace render {
	// @TODO ... This needs a lot of thinking
	//      Maybe copy the data too
	//      Maybe do something more
	//      Maybe allocate directly on gpu memory

	// @TODO ? For now dont merge allocate functions, first need solve ^TODO

	// @TODO @CLEANUP maybe clean up weird size counting^^
	static inline char* ReserveResourceCommand(RenderContextStream* contextStream, const void* data, u64 size, CommandType type) {
		u64 realSize = sizeof(AllocationCommand) + size - sizeof(Resource);

		char* buffer = contextStream->ReserveSize(realSize);
		AllocationCommand* job = (AllocationCommand*) buffer;
		job->commandType = type;
		core::Memcpy(&job->resource, data, size);
		return buffer;
	}

	static inline char* AllocateShader(RenderContextStream* contextStream, const Resource* resource) {
		const Shader* shader = static_cast<const Shader*>(resource);
		return ReserveResourceCommand(contextStream, shader, sizeof(Shader), CommandType::RESOURCE_ALLOCATION);
	}

	static inline char* AllocateTexture(RenderContextStream* contextStream, const Resource* resource) {
		const Texture* texture = static_cast<const Texture*>(resource);
		return ReserveResourceCommand(contextStream, texture, sizeof(Texture), CommandType::RESOURCE_ALLOCATION);
	}

	static inline char* AllocateVertexBuffer(RenderContextStream* contextStream, const Resource* resource) {
		const VertexBuffer* vb = static_cast<const VertexBuffer*>(resource);
		return ReserveResourceCommand(contextStream, vb, sizeof(VertexBuffer), CommandType::RESOURCE_ALLOCATION);
	}

	static inline char* AllocateVertexBufferInfo(RenderContextStream* contextStream, const Resource* resource) {
		const VertexDescription* vbi = static_cast<const VertexDescription*>(resource);
		return ReserveResourceCommand(contextStream, vbi, sizeof(VertexDescription), CommandType::RESOURCE_ALLOCATION);
	}

	static inline char* AllocateIndexBuffer(RenderContextStream* contextStream, const Resource* resource) {
		const IndexBuffer* ib = static_cast<const IndexBuffer*>(resource);
		return ReserveResourceCommand(contextStream, ib, sizeof(IndexBuffer), CommandType::RESOURCE_ALLOCATION);
	}

	static inline char* DeallocateResource(RenderContextStream* contextStream, const Resource* resource) {
		return ReserveResourceCommand(contextStream, resource, sizeof(Resource), CommandType::RESOURCE_DEALLOCATION);
	}

	RenderResourceContext::RenderResourceContext() :
		_buffer(nullptr) {
	}

	void RenderResourceContext::Allocate(RenderContextStream* contextStream, Resource* resource) {
		ASSERT(!_buffer);

		switch (ResourceGetType(resource)) {
		case ResourceType::SHADER: _buffer = AllocateShader(contextStream, resource); break;
		case ResourceType::TEXTURE: _buffer = AllocateTexture(contextStream, resource); break;
		case ResourceType::VERTEX_BUFFER: _buffer = AllocateVertexBuffer(contextStream, resource); break;
		case ResourceType::VERTEX_DESCRIPTION: _buffer = AllocateVertexBufferInfo(contextStream, resource); break;
		case ResourceType::INDEX_BUFFER: _buffer = AllocateIndexBuffer(contextStream, resource); break;
		default:
			ASSERT(false);
			break;
		}
	}

	void RenderResourceContext::Deallocate(RenderContextStream* contextStream, Resource* resource) {
		ASSERT(!_buffer);

		switch (ResourceGetType(resource)) {
		case ResourceType::SHADER:
		case ResourceType::TEXTURE:
		case ResourceType::VERTEX_BUFFER:
		case ResourceType::VERTEX_DESCRIPTION:
		case ResourceType::INDEX_BUFFER:
			_buffer = DeallocateResource(contextStream, resource);
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	const char* RenderResourceContext::GetData() const {
		return _buffer;
	}
}
