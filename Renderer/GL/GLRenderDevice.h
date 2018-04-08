#pragma once

#include <Core/Common/Types.h>
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

	// @TODO initialisation is stil not done (as SDL2 is still used)

	// @TODO make resource creation asynchronous

	struct ShaderData;

	// Used for hashing uniformnames
	using HashFunction = h64(*)(const void* key, u32 length);

	class GLRenderDevice : public IRenderDevice {
	public:
		GLRenderDevice();
		~GLRenderDevice() override;

		void Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction);

		virtual void Render(const RenderContext* rc, u64 count) override;
		virtual void ManageResources(const RenderResourceContext* rrc, u64 count) override;

	private:
		void ProcessCommand(const CommandProxy* proxy);
		void ProcessJobPackage(const JobPackage* jobPackage);
		void ProcessJobPackageResources(const Resource* resourceBegin, const Resource* resourceEnd);
		void ProcessJobPackageResource(const Resource* resource, u32& inOutTextureCount);
		void ProcessJobPackageUniforms(const void* data, u32 count, const ShaderData& shaderData);
		const void* ProcessJobPackageUniform(const void* data, const ShaderData& shaderData);

		void ProcessJobPackageBatch(const BatchDescription* batch);

		void AllocateShader(const Shader* shader);
		void AllocateTexture(const Texture* texture);
		void AllocateVertexBuffer(const VertexBuffer* vb);
		void AllocateVertexDescription(const VertexDescription* vbd);
		void AllocateIndexBuffer(const IndexBuffer* ib);

		void DeallocateShader(const Resource* shader);
		void DeallocateTexture(const Resource* texture);
		void DeallocateVertexBuffer(const Resource* vb);
		void DeallocateVertexInformation(const Resource* vbi);
		void DeallocateIndexBuffer(const Resource* ib);

	private:
		u64 _bufferSize;
		u64 _bufferCapacity;

		char* _commandBuffer;

		core::IAllocator* _frameAllocator;
		core::IAllocator* _resourcesAllocator;

		HashFunction _hashFunction;

		core::LookupArray<ShaderData> _shaders;
		core::LookupArray<u32> _textures;
		core::LookupArray<u32> _vertexDescriptions;
		core::LookupArray<u32> _vertexBuffers;
		core::LookupArray<u32> _indexBuffers;

		// @TODO ? Not liking this extra indirection
		// (Engine is creating handles, but maybe render device should (but how to give them back safely))
		core::HashMap<core::Handle> _handleToIndexMap;
	};
}



