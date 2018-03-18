#pragma once

#include <Core/Types.h>
#include <Core/Allocator/IAllocator.h>
#include <Core/Collection/HashMap.h>
#include <Core/Collection/LookupArray.h>

#include "Renderer/API/IRenderDevice.h"

namespace render {
	class RenderContext;
	class RenderResourceContext;

	struct CommandProxy;
	struct Resource;
	struct JobPackage;
	struct BatchDescription;

	struct Shader;
	struct Texture;
	struct VertexBuffer;
	struct VertexDescription;
	struct IndexBuffer;
}

namespace gl {

	struct ShaderReflection;
	struct ShaderData;

	// Used for hashing uniformnames
	using HashFunction = core::Hash(*)(const void* key, u32 length);

	// @TODO allocations!!!
	// @TODO allocations!!!
	// @TODO allocations!!!
	class GLRenderDevice : public render::IRenderDevice {
	public:
		GLRenderDevice();
		~GLRenderDevice() override;

		void Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction);

		virtual void Render(const render::RenderContext* rc, u64 count) override;
		virtual void ManageResources(const render::RenderResourceContext* rrc, u64 count) override;

	private:
		void ProcessCommand(const render::CommandProxy* proxy);
		void ProcessJobPackage(const render::JobPackage* jobPackage);
		void ProcessJobPackageResources(const render::Resource* resourceBegin, const render::Resource* resourceEnd);
		void ProcessJobPackageResource(const render::Resource* resource, u32& inOutTextureCount);
		void ProcessJobPackageUniforms(const void* data, u32 count, u32 reflectionIndex);
		const void* ProcessJobPackageUniform(const void* data, ShaderReflection* reflection);

		void ProcessJobPackageBatch(const render::BatchDescription* batch);

		void AllocateShader(const render::Shader* shader);
		void AllocateTexture(const render::Texture* texture);
		void AllocateVertexBuffer(const render::VertexBuffer* vb);
		void AllocateVertexDescription(const render::VertexDescription* vbd);
		void AllocateIndexBuffer(const render::IndexBuffer* ib);

		void DeallocateShader(const render::Resource* shader);
		void DeallocateTexture(const render::Resource* texture);
		void DeallocateVertexBuffer(const render::Resource* vb);
		void DeallocateVertexInformation(const render::Resource* vbi);
		void DeallocateIndexBuffer(const render::Resource* ib);

	private:
		u64 _bufferSize;
		u64 _bufferCapacity;

		char* _commandBuffer;

		core::IAllocator* _frameAllocator;
		core::IAllocator* _resourcesAllocator;

		HashFunction _hashFunction;

		core::LookupArray<ShaderReflection> _shaderReflection;

		core::HashMap<ShaderData> _shaders;
		core::HashMap<u32> _textures;
		core::HashMap<u32> _vertexDescriptions;
		core::HashMap<u32> _vertexBuffers;
		core::HashMap<u32> _indexBuffers;
	};
}



