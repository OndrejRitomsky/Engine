#include "GLRenderDevice.h"

#include <Core/Common/Utility.h>
#include <Core/Collection/LookupArray.inl>
#include <Core/Collection/HashMap.inl>

#include "Renderer/API/RenderContext.h"
#include "Renderer/API/RenderResourceContext.h"

#include "Renderer/API/Command/JobPackage.h"
#include "Renderer/API/Command/Uniform.h"

#include "Renderer/API/Internal/Commands.h"

#include "Renderer/API/Resources/Buffer.h"
#include "Renderer/API/Resources/Texture.h"
#include "Renderer/API/Resources/Shader.h"
#include "Renderer/API/Resources/Resource.h"

#include "Renderer/GL/Internal/GLWrapper.h"
#include "Renderer/GL/Internal/GLUtil.h"

namespace render {
	struct UniformData {
		UniformType type;
		u32 glUniformType;

		i32 location;
		i32 size;
		i32 length;
	};

	// Not liking the hash map for uniforms, but to change that shader frontend is needed
	// @TODO fix allocator for uniformData hashmap it should be pooled
	struct ShaderData {
		u32 shader;
		core::HashMap<UniformData> uniformData;
	};

	//---------------------------------------------------------------------------
	GLRenderDevice::GLRenderDevice() :
		_commandBuffer(nullptr),
		_bufferSize(0),
		_bufferCapacity(0),
		_frameAllocator(nullptr),
		_hashFunction(nullptr) {
	}

	//---------------------------------------------------------------------------
	GLRenderDevice::~GLRenderDevice() {

		if (_commandBuffer)
			_resourcesAllocator->Deallocate(_commandBuffer);

		ASSERT2(_handleToIndexMap.Count() == 0, "Deallocate commands must be sent!");

		auto it = _handleToIndexMap.CIterator();
		for (u32 i = 0; i < it.count; ++i) {
			const Resource* resource = reinterpret_cast<const Resource*>(&it.keys[i]);
			switch (ResourceGetType(resource)) {
			case ResourceType::SHADER:
			{
				const ShaderData& shaderData = _shaders.Get(it.values[i]);
				gl::program::Delete(shaderData.shader);
				_shaders.Remove(it.values[i]);
				break;
			}
			case ResourceType::TEXTURE:
			{
				u32 index = _textures.Get(it.values[i]);
				gl::texture::Delete(index); 
				_textures.Remove(it.values[i]);
				break;
			}
			case ResourceType::VERTEX_BUFFER:
			{
				u32 index = _vertexBuffers.Get(it.values[i]);
				gl::vertex::DeleteBuffers((u32*) &index, 1);
				_vertexBuffers.Remove(it.values[i]);
				break;
			}
			case ResourceType::VERTEX_DESCRIPTION:
			{
				u32 index = _vertexDescriptions.Get(it.values[i]);
				gl::vertex::DeleteArrayObjects((u32*) &index, 1);
				_vertexDescriptions.Remove(it.values[i]);
				break;
			}
			case ResourceType::INDEX_BUFFER: 
			{
				u32 index = _indexBuffers.Get(it.values[i]);
				gl::vertex::DeleteBuffers((u32*) &index, 1);
				_indexBuffers.Remove(it.values[i]);
				break;
			}
			default: ASSERT(false); break;
			}
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction) {
		_frameAllocator = frameAllocator;
		_resourcesAllocator = resourcesAllocator;

		_hashFunction = hashFunction;

		_shaders.Init(resourcesAllocator, 128);
		_textures.Init(resourcesAllocator, 128);
		_vertexDescriptions.Init(resourcesAllocator, 128);
		_vertexBuffers.Init(resourcesAllocator, 128);
		_indexBuffers.Init(resourcesAllocator, 128);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::Render(const RenderContext* rc, u64 count) {
		if (!_bufferCapacity) {
			// @TODO allocators should have out capacity
			_bufferCapacity = 1024 * 1024;

			_commandBuffer = static_cast<char*>(_resourcesAllocator->Allocate(_bufferCapacity, alignof(char)));
		}

		for (u64 i = 0; i < count; ++i) {
			u32 commandsCount;
			const CommandProxy* proxies = rc[i].GetCommands(commandsCount);

			u64 commandsSize = commandsCount * sizeof(CommandProxy);
			if (_bufferSize + commandsSize > _bufferCapacity) {
				_bufferCapacity *= 2;

				char* result = static_cast<char*>(_resourcesAllocator->Allocate(_bufferCapacity, alignof(char)));
				Memcpy(result, _commandBuffer, _bufferSize);
				_resourcesAllocator->Deallocate(_commandBuffer);
				_commandBuffer = result;
			}

			Memcpy(_commandBuffer + _bufferSize, proxies, commandsSize);
			_bufferSize += commandsSize;
		}

		// @TODO ^^
		// Who should sort and instance commands, render device or engine pipeline

		CommandProxy* proxy = reinterpret_cast<CommandProxy*>(_commandBuffer);
		CommandProxy* proxyEnd = proxy + _bufferSize / sizeof(CommandProxy);

		for (; proxy < proxyEnd; ++proxy)
			ProcessCommand(proxy);

		_bufferSize = 0;
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ManageResources(const RenderResourceContext* rrc, u64 count) {
		for (u32 i = 0; i < count; ++i) {
			const char* data = rrc[i].GetData();
			const CommandHeader* commandHeader = reinterpret_cast<const CommandHeader*>(data);

			if (commandHeader->commandType == CommandType::RESOURCE_ALLOCATION) {
				const AllocationCommand* command = static_cast<const AllocationCommand*>(commandHeader);
				const Resource* resource = &command->resource;

				switch (ResourceGetType(resource)) {
				case ResourceType::SHADER: AllocateShader(static_cast<const Shader*>(resource)); break;
				case ResourceType::TEXTURE:	AllocateTexture(static_cast<const Texture*>(resource));	break;
				case ResourceType::VERTEX_BUFFER: AllocateVertexBuffer(static_cast<const VertexBuffer*>(resource)); break;
				case ResourceType::VERTEX_DESCRIPTION: AllocateVertexDescription(static_cast<const VertexDescription*>(resource)); break;
				case ResourceType::INDEX_BUFFER: AllocateIndexBuffer(static_cast<const IndexBuffer*>(resource)); break;
				default: ASSERT(false); break;
				}
			}
			else if (commandHeader->commandType == CommandType::RESOURCE_DEALLOCATION) {
				const DeallocationCommand* command = static_cast<const DeallocationCommand*>(commandHeader);
				const Resource* resource = &command->resource;

				switch (ResourceGetType(resource)) {
				case ResourceType::SHADER: DeallocateShader(resource); break;
				case ResourceType::TEXTURE:	DeallocateTexture(resource); break;
				case ResourceType::VERTEX_BUFFER: DeallocateVertexBuffer(resource); break;
				case ResourceType::VERTEX_DESCRIPTION: DeallocateVertexInformation(resource); break;
				case ResourceType::INDEX_BUFFER: DeallocateIndexBuffer(resource); break;
				default: ASSERT(false); break;
				}
			}
			else {
				ASSERT(false);
			}
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessCommand(const CommandProxy* proxy) {
		const CommandHeader* commandHeader = reinterpret_cast<const CommandHeader*>(proxy->data);

		switch (commandHeader->commandType) {
		case CommandType::RENDER:
		{
			const RenderJobCommand* renderCommand = static_cast<const RenderJobCommand*>(commandHeader);
			ProcessJobPackage(&renderCommand->jobPackage);
			break;
		}
		case CommandType::CLEAR:
		{
			u32 fl = static_cast<const ClearCommand*>(commandHeader)->flags;
			f32 color[4] = {0,0,0,1};

			gl::draw::ClearColor(color);
			gl::draw::Clear((fl & ClearFlags::COLOR) > 0, (fl & ClearFlags::DEPTH) > 0, (fl & ClearFlags::STENCIL) > 0);
			break;
		}
		case CommandType::RESOURCE_ALLOCATION:
		case CommandType::RESOURCE_DEALLOCATION:
		default:
			ASSERT(false);
			break;
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackage(const JobPackage* jobPackage) {
		const char* resourceData = reinterpret_cast<const char*>(jobPackage) + jobPackage->resourcesOffset;
		const Resource* resource = reinterpret_cast<const Resource*>(resourceData);
		const Resource* resourceEnd = reinterpret_cast<const Resource*>(resourceData + jobPackage->resourcesCount);


		const char* uniforms = reinterpret_cast<const char*>(jobPackage) + jobPackage->uniformsOffset;

		ASSERT((char*) resourceEnd <= (char*) jobPackage + jobPackage->size);

		const Resource* shaderResource = &jobPackage->shader;
		ASSERT(ResourceGetType(shaderResource) == ResourceType::SHADER);

		const core::Handle* handle = _handleToIndexMap.Find(shaderResource->handle);
		ASSERT(handle);

		const ShaderData& shaderData = _shaders.Get(*handle);
		gl::program::Use(shaderData.shader);

		ProcessJobPackageResources(resource, resourceEnd);
		ProcessJobPackageUniforms(uniforms, jobPackage->uniformsCount, shaderData);
		ProcessJobPackageBatch(&jobPackage->batch);
		gl::vertex::UnbindArrayObject();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageResources(const Resource* resourceBegin, const Resource* resourceEnd) {
		u32 texturesCount = 0;

		// OpenGL specific find VAO first!
		const Resource* vertexDescription = nullptr;
		for (const Resource* resource = resourceBegin; resource < resourceEnd; ++resource) {
			if (ResourceGetType(resource) == ResourceType::VERTEX_DESCRIPTION) {
				vertexDescription = resource;
				break;
			}
		}

		const Resource* resource = resourceBegin;

		if (vertexDescription) {
			ProcessJobPackageResource(vertexDescription, texturesCount);

			for (; resource < vertexDescription; ++resource)
				ProcessJobPackageResource(resource, texturesCount);

			resource = vertexDescription + 1;
		} // else -> Not using VAO is for now allowed

		for (; resource < resourceEnd; ++resource)
			ProcessJobPackageResource(resource, texturesCount);
	}

	void GLRenderDevice::ProcessJobPackageResource(const Resource* resource, u32& inOutTextureCount) {
		const core::Handle* handle = _handleToIndexMap.Find(resource->handle);
		ASSERT(handle);
		
		switch (ResourceGetType(resource)) {
		case ResourceType::SHADER: 
			ASSERT(false); 
			break;
		case ResourceType::TEXTURE:
		{
			u32 texture = _textures.Get(*handle);
			gl::texture::SetActiveTexture(inOutTextureCount++); // @TODO find out if seting every frame is ok
			gl::texture::Bind(texture);
			break;
		}
		case ResourceType::VERTEX_BUFFER:
		{
			u32 vb = _vertexBuffers.Get(*handle);
			gl::vertex::BindArrayBuffer(vb);
			break;
		}
		case ResourceType::VERTEX_DESCRIPTION:
		{
			u32 vb = _vertexDescriptions.Get(*handle);
			gl::vertex::BindArrayObject(vb);
			break;
		}
		case ResourceType::INDEX_BUFFER:
		{
			u32 ib = _indexBuffers.Get(*handle);
			gl::vertex::BindIndexArrayBuffer(ib);
			break;
		}
		default: ASSERT(false); break;
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageUniforms(const void* data, u32 count, const ShaderData& shaderData) {
		const void* current = data;
		u32 i = 0;
		while (i++ < count)
			current = ProcessJobPackageUniform(current, shaderData);
	}

	//---------------------------------------------------------------------------
	const void* GLRenderDevice::ProcessJobPackageUniform(const void* data, const ShaderData& shaderData) {
		const UniformHeader* header = static_cast<const UniformHeader*>(data);
		
		const UniformData* uniform = shaderData.uniformData.Find(header->nameHash);

		ASSERT(uniform);
		const char* result = static_cast<const char*>(data);

		switch (uniform->type) {
		case UniformType::FLOAT:
			gl::uniform::Set1f(uniform->location, reinterpret_cast<const FloatUniform*>(header)->value);
			result += sizeof(FloatUniform);
			break;
		case UniformType::INT:
			gl::uniform::Set1i(uniform->location, reinterpret_cast<const IntUniform*>(header)->value);
			result += sizeof(IntUniform);
			break;
		case UniformType::VECTOR2:
			gl::uniform::Set2f(uniform->location, reinterpret_cast<const Vector2Uniform*>(header)->values);
			result += sizeof(Vector2Uniform);
			break;
		case UniformType::VECTOR3:
			gl::uniform::Set3f(uniform->location, reinterpret_cast<const Vector3Uniform*>(header)->values);
			result += sizeof(Vector3Uniform);
			break;
		case UniformType::MATRIX:
			gl::uniform::SetMatrix4f(uniform->location, reinterpret_cast<const MatrixUniform*>(header)->values);
			result += sizeof(MatrixUniform);
			break;
		case UniformType::SAMPLER2D:
		{
			core::Handle* handle = _handleToIndexMap.Find(reinterpret_cast<const Sampler2DUniform*>(header)->handle);
			u32 glID = _textures.Get(*handle);
			gl::texture::SetActiveTexture(0); // @TODO find out if seting every frame is ok
			gl::texture::Bind(glID);
			result += sizeof(Sampler2DUniform);
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		return result;
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageBatch(const BatchDescription* batch) {
		switch (batch->type) {
		case BatchType::LINES:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					gl::draw::LineElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // LineArrayInstanced
			}
			else {
				if (batch->indices > 0)
					gl::draw::LineElements(batch->indices, batch->indexType);
				else
					gl::draw::LineArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		case BatchType::LINE_LOOP:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					gl::draw::LineLoopElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // LineLoopArrayInstanced
			}
			else {
				if (batch->indices > 0)
					gl::draw::LineLoopElements(batch->indices, batch->indexType);
				else
					gl::draw::LineLoopArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		case BatchType::TRIANGLES:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					gl::draw::TriangleElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // TriangleArrayInstanced
			}
			else {
				if (batch->indices > 0)
					gl::draw::TriangleElements(batch->indices, batch->indexType);
				else
					gl::draw::TriangleArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateShader(const Shader* shader) {
		ASSERT(!_handleToIndexMap.Find(shader->handle));

		u32 vs = gl::util::CompileVertexShader(shader->vertexStage.data);
		u32 fs = gl::util::CompileFragmentShader(shader->fragmentStage.data);
		u32 program = gl::util::CreateProgram(vs, fs);

		gl::shader::Delete(vs);
		gl::shader::Delete(fs);
		
		core::Handle handle;
		{
			ShaderData shaderData;
			shaderData.shader = program;
			shaderData.uniformData.Init(_resourcesAllocator);

			handle = _shaders.Add(core::move(shaderData));
			_handleToIndexMap.Add(shader->handle, handle);
		}
		

		// @TODO FIX this

		core::HashMap<UniformData>& uniformMap = _shaders.Get(handle).uniformData;

		i32 count = gl::uniform::Count(program);
		for (i32 i = 0; i < count; ++i) {
			i32 length;
			char buffer[64];

			UniformData data;
			gl::uniform::Info(program, i, buffer, 64, length, data.size, data.glUniformType);
			
			data.location = gl::uniform::Location(program, buffer);
			data.type = gl::util::GLTypeToUniformType(data.glUniformType);
			uniformMap.Add(_hashFunction(buffer, length), core::move(data));
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateTexture(const Texture* texture) {
		ASSERT(!_handleToIndexMap.Find(texture->handle));

		u32 glID = gl::texture::Create();
		gl::texture::Bind(glID);
		gl::texture::SetParameterMinMagFilter(texture->minFilter, texture->maxFilter);
		gl::texture::SetParameterWrapST(texture->wrapS, texture->wrapT);
		gl::texture::SetData(texture->data, texture->width, texture->height, texture->mipmaps, texture->format);
		gl::texture::Unbind();

		core::Handle handle = _textures.Add(glID);
		_handleToIndexMap.Add(texture->handle, handle);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexBuffer(const VertexBuffer* vb) {
		ASSERT(!_handleToIndexMap.Find(vb->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, (u32*) &glID);
		gl::vertex::BindArrayBuffer(glID);

		if (vb->bufferType == BufferType::STATIC) {
			gl::vertex::ArrayBufferStaticData(vb->data, vb->dataSize);
		}
		else if (vb->bufferType == BufferType::DYNAMIC_UPDATE) {
			gl::vertex::ArrayBufferDynamicData(vb->data, vb->dataSize);
		}

		core::Handle handle = _vertexBuffers.Add(glID);
		_handleToIndexMap.Add(vb->handle, handle);

		gl::vertex::UnbindArrayBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexDescription(const VertexDescription* vbd) {
		ASSERT(!_handleToIndexMap.Find(vbd->handle));
		// @TODO? current requirement: everything mentioned by vbd has to be already in GPU -> maybe rework

		u32 glID;
		gl::vertex::CreateArrayObjects(1, &glID);
		gl::vertex::BindArrayObject(glID);

		if (vbd->hasIndexBuffer) {
			core::Handle* handle = _handleToIndexMap.Find(vbd->indexBufferHandle);
			u32 ibID = _indexBuffers.Get(*handle);
			gl::vertex::BindIndexArrayBuffer(ibID);
		}

		u64 lastVbh = U64MAX;
		u32 vbID;
		for (u32 i = 0; i < vbd->attributesCount; ++i) {
			const VertexAttribute* attribute = &vbd->attributes[i];

			u64 vbh = attribute->vertexBufferHandle;
			if (vbh != lastVbh) {
				core::Handle* handle = _handleToIndexMap.Find(attribute->vertexBufferHandle);
				u32 newVbID = _vertexBuffers.Get(*handle);

				vbID = newVbID;
				gl::vertex::BindArrayBuffer(vbID);
			}

			switch (attribute->type) {
			case VertexAttributeType::F32: 
				gl::vertex::EnableAttributeArray(attribute->index);
				gl::vertex::AttributePointerFloat(attribute->index, 1, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					gl::vertex::AttributeDivisor(attribute->index, attribute->divisor);
				break;
			case VertexAttributeType::V2:
				gl::vertex::EnableAttributeArray(attribute->index);
				gl::vertex::AttributePointerFloat(attribute->index, 2, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					gl::vertex::AttributeDivisor(attribute->index, attribute->divisor);
				break;
			case VertexAttributeType::M4:
				for (u32 i = 0; i < 4; ++i) {
					u32 index = attribute->index + i;
					gl::vertex::EnableAttributeArray(index + i);
					gl::vertex::AttributePointerFloat(index + i, 4, attribute->stride, attribute->offset + 4 * sizeof(f32) * i, attribute->normalize);
					if (attribute->divisor > 0)
						gl::vertex::AttributeDivisor(index, attribute->divisor);
				}
				break;
			default: 
				ASSERT(false);
				break;
			}
		}

		//gl::vertex::UnbindIndexBuffer();
		//gl::vertex::UnbindArrayBuffer();
		gl::vertex::UnbindArrayObject();
		
		_handleToIndexMap.Add(vbd->handle, _vertexDescriptions.Add(glID));
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateIndexBuffer(const IndexBuffer* ib) {
		ASSERT(!_handleToIndexMap.Find(ib->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, (u32*) &glID);
		gl::vertex::BindIndexArrayBuffer(glID);

		if (ib->bufferType == BufferType::STATIC) {
			gl::vertex::IndexArrayBufferStaticData(ib->data, ib->dataSize);
		}
		else if (ib->bufferType == BufferType::DYNAMIC_UPDATE) {
			gl::vertex::IndexArrayBufferDynamicData(ib->data, ib->dataSize);
		}

		_handleToIndexMap.Add(ib->handle, _indexBuffers.Add(glID));

		gl::vertex::UnbindIndexBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateShader(const Resource* shader) {
		core::Handle* handle = _handleToIndexMap.Find(shader->handle);
		ASSERT(handle);

		if (handle) {
			ShaderData& shaderData = _shaders.Get(*handle);

			gl::program::Delete(shaderData.shader);

			_handleToIndexMap.SwapRemove(shader->handle);
			_shaders.Remove(*handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateTexture(const Resource* texture) {
		core::Handle* handle = _handleToIndexMap.Find(texture->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _textures.Get(*handle);
			gl::program::Delete(glID);

			_handleToIndexMap.SwapRemove(texture->handle);
			_textures.Remove(*handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexBuffer(const Resource* vb) {
		core::Handle* handle = _handleToIndexMap.Find(vb->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _vertexBuffers.Get(*handle);
			gl::vertex::DeleteBuffers(&glID, 1);

			_handleToIndexMap.SwapRemove(vb->handle);
			_vertexBuffers.Remove(*handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexInformation(const Resource* vbi) {
		core::Handle* handle = _handleToIndexMap.Find(vbi->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _vertexDescriptions.Get(*handle);
			gl::vertex::DeleteArrayObjects(&glID, 1);

			_handleToIndexMap.SwapRemove(vbi->handle);
			_vertexDescriptions.Remove(*handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateIndexBuffer(const Resource* ib) {
		core::Handle* handle = _handleToIndexMap.Find(ib->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _indexBuffers.Get(*handle);
			gl::vertex::DeleteBuffers(&glID, 1);

			_handleToIndexMap.SwapRemove(ib->handle);
			_indexBuffers.Remove(*handle);
		}
	}
}


