#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/lookuparray.h>

#include "../api/irender_device.h"

namespace core {
	struct IAllocator;
}

namespace render {
	class RenderContext;
	struct ResourceContext;

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

	// @TODO make resource creation asynchronous ... the data would have to be copied cpu hold it only for one / two frames

	struct ShaderData;

	// Used for hashing uniformnames
	using HashFunction = h64(*)(const void* key, u32 length);

	class GLRenderDevice : public IRenderDevice {
	public:
		GLRenderDevice();
		~GLRenderDevice() override;

		void Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction, void* deviceContext);

		virtual void Render(const RenderContext* rc, u64 count) override;
		virtual void ManageResources(const ResourceContext* rrc, u64 count) override;

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

		bool GLInit(void* deviceContext);

	private:
		u64 _bufferSize;
		u64 _bufferCapacity;

		char* _commandBuffer;

		void* _renderDeviceContext;
		void* _deviceContext;

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



