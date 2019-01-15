#include "resource_context.h"

#include <core/allocator/allocators.h>
#include <core/algorithm/cstring.h>

#include "render_context_stream.h"
#include "render_resources.h"

#include "internal/commands.h"


namespace render {
	// @TODO ... This needs thinknig
	//   Maybe copy the data too
	//   Maybe do something more
	//   Maybe allocate directly on gpu memory


	static Resource* ReserveResourceCommand(RenderContextStream* contextStream, const void* data, u64 size, u64 alignment) {
		Resource* buffer = (Resource*) contextStream->ReserveSize(size, alignment);
		core::Memcpy(buffer, data, size);
		return buffer;
	}

	ResourceContext ShaderAllocate(RenderContextStream* contextStream, const Shader* shader) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, shader, sizeof(Shader), alignof(Shader));
		context.type = ResourceCommandType::RESOURCE_ALLOCATION;
		return context;
	}

	ResourceContext TextureAllocate(RenderContextStream* contextStream, const Texture* texture) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, texture, sizeof(Texture), alignof(Texture));
		context.type = ResourceCommandType::RESOURCE_ALLOCATION;
		return context;
	}

	ResourceContext VertexBufferAllocate(RenderContextStream* contextStream, const VertexBuffer* vertexBuffer) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, vertexBuffer, sizeof(VertexBuffer), alignof(VertexBuffer));
		context.type = ResourceCommandType::RESOURCE_ALLOCATION;
		return context;
	}

	ResourceContext VertexDescriptionAllocate(RenderContextStream* contextStream, const VertexDescription* vertexDesciption) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, vertexDesciption, sizeof(VertexDescription), alignof(VertexDescription));
		context.type = ResourceCommandType::RESOURCE_ALLOCATION;
		return context;
	}

	ResourceContext IndexBufferAllocate(RenderContextStream* contextStream, const IndexBuffer* indexBuffer) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, indexBuffer, sizeof(IndexBuffer), alignof(IndexBuffer));
		context.type = ResourceCommandType::RESOURCE_ALLOCATION;
		return context;
	}

	ResourceContext ResourceDeallocate(RenderContextStream* contextStream, const Resource* resource) {
		ResourceContext context;
		context.resource = ReserveResourceCommand(contextStream, resource, sizeof(Resource), alignof(Resource));
		context.type = ResourceCommandType::RESOURCE_DEALLOCATION;
		return context;
	}
}
