#pragma once

#include "command/command_proxy.h"

namespace render {
	struct Resource;
	struct Shader;
	struct Texture;
	struct VertexBuffer;
	struct IndexBuffer;
	struct VertexDescription;
	
	class RenderContextStream;

	// This is internal, could be moved if soa
	enum class ResourceCommandType : u8 {
		RESOURCE_ALLOCATION,
		RESOURCE_DEALLOCATION
	};

	// CAREFUL! resource header is copied but with it its pointer to the resource data
	// pointer to the data has to be valid until render happens!!! 


	// the pipeline could have be changed to array of types and array of resrouce
	// one resource, allocation can be casted to subtype, deallocation cannot
	struct ResourceContext {
		ResourceCommandType type;
		Resource* resource; 
	};


	ResourceContext ShaderAllocate(RenderContextStream* contextStream, const Shader* shader);

	ResourceContext TextureAllocate(RenderContextStream* contextStream, const Texture* texture);

	ResourceContext VertexBufferAllocate(RenderContextStream* contextStream, const VertexBuffer* vertexBuffer);

	ResourceContext IndexBufferAllocate(RenderContextStream* contextStream, const IndexBuffer* indexBuffer);

	ResourceContext VertexDescriptionAllocate(RenderContextStream* contextStream, const VertexDescription* vertexDesciption);


	ResourceContext ResourceDeallocate(RenderContextStream* contextStream, const Resource* resource);
}