#include "GLRenderDevice.h"

#include <Core/Utility.h>
#include <Core/Allocator/Reallocation.h>
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

namespace gl {
	struct UniformData {
		render::UniformType type;
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

		for (auto& it = _handleToIndexMap.cKeyValueBegin(); it != _handleToIndexMap.cKeyValueEnd(); ++it) {
			const render::Resource* resource = reinterpret_cast<const render::Resource*>(&it.key);
			switch (render::ResourceGetType(resource)) {
			case render::ResourceType::SHADER:
			{
				const ShaderData& shaderData = _shaders.Get(it.value);
				gl::program::Delete(shaderData.shader);
				_shaders.Remove(it.value);
				break;
			}
			case render::ResourceType::TEXTURE:
			{
				u32 index = _textures.Get(it.value);
				gl::texture::Delete(index); 
				_textures.Remove(it.value);
				break;
			}
			case render::ResourceType::VERTEX_BUFFER:
			{
				u32 index = _vertexBuffers.Get(it.value);
				gl::vertex::DeleteBuffers(&index, 1);
				_vertexBuffers.Remove(it.value);
				break;
			}
			case render::ResourceType::VERTEX_DESCRIPTION:
			{
				u32 index = _vertexDescriptions.Get(it.value);
				gl::vertex::DeleteArrayObjects(&index, 1);
				_vertexDescriptions.Remove(it.value);
				break;
			}
			case render::ResourceType::INDEX_BUFFER: 
			{
				u32 index = _indexBuffers.Get(it.value);
				gl::vertex::DeleteBuffers(&index, 1);
				_indexBuffers.Remove(it.value);
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
	void GLRenderDevice::Render(const render::RenderContext* rc, u64 count) {
		if (!_bufferCapacity) {
			// @TODO allocators should have out capacity
			_bufferCapacity = 1024 * 1024;
			_commandBuffer = static_cast<char*>(core::ReallocateMemCopy(_resourcesAllocator, nullptr, 0, alignof(u64), _bufferCapacity));
		}

		for (u64 i = 0; i < count; ++i) {
			u32 commandsCount;
			const render::CommandProxy* proxies = rc[i].GetCommands(commandsCount);

			u64 commandsSize = commandsCount * sizeof(render::CommandProxy);
			if (_bufferSize + commandsSize > _bufferCapacity) {
				_bufferCapacity *= 2;
				_commandBuffer = static_cast<char*>(core::ReallocateMemCopy(_resourcesAllocator, _commandBuffer, _bufferSize, alignof(u64), _bufferCapacity));
			}

			memcpy(_commandBuffer + _bufferSize, proxies, commandsSize);
			_bufferSize += commandsSize;
		}

		// @TODO ^^
		// Who should sort and instance commands, render device or engine pipeline

		render::CommandProxy* proxy = reinterpret_cast<render::CommandProxy*>(_commandBuffer);
		render::CommandProxy* proxyEnd = proxy + _bufferSize / sizeof(render::CommandProxy);

		for (; proxy < proxyEnd; ++proxy)
			ProcessCommand(proxy);

		_bufferSize = 0;
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ManageResources(const render::RenderResourceContext* rrc, u64 count) {
		for (u32 i = 0; i < count; ++i) {
			const char* data = rrc[i].GetData();
			const render::CommandHeader* commandHeader = reinterpret_cast<const render::CommandHeader*>(data);

			if (commandHeader->commandType == render::CommandType::RESOURCE_ALLOCATION) {
				const render::AllocationCommand* command = static_cast<const render::AllocationCommand*>(commandHeader);
				const render::Resource* resource = &command->resource;

				switch (render::ResourceGetType(resource)) {
				case render::ResourceType::SHADER: AllocateShader(static_cast<const render::Shader*>(resource)); break;
				case render::ResourceType::TEXTURE:	AllocateTexture(static_cast<const render::Texture*>(resource));	break;
				case render::ResourceType::VERTEX_BUFFER: AllocateVertexBuffer(static_cast<const render::VertexBuffer*>(resource)); break;
				case render::ResourceType::VERTEX_DESCRIPTION: AllocateVertexDescription(static_cast<const render::VertexDescription*>(resource)); break;
				case render::ResourceType::INDEX_BUFFER: AllocateIndexBuffer(static_cast<const render::IndexBuffer*>(resource)); break;
				default: ASSERT(false); break;
				}
			}
			else if (commandHeader->commandType == render::CommandType::RESOURCE_DEALLOCATION) {
				const render::DeallocationCommand* command = static_cast<const render::DeallocationCommand*>(commandHeader);
				const render::Resource* resource = &command->resource;

				switch (render::ResourceGetType(resource)) {
				case render::ResourceType::SHADER: DeallocateShader(resource); break;
				case render::ResourceType::TEXTURE:	DeallocateTexture(resource); break;
				case render::ResourceType::VERTEX_BUFFER: DeallocateVertexBuffer(resource); break;
				case render::ResourceType::VERTEX_DESCRIPTION: DeallocateVertexInformation(resource); break;
				case render::ResourceType::INDEX_BUFFER: DeallocateIndexBuffer(resource); break;
				default: ASSERT(false); break;
				}
			}
			else {
				ASSERT(false);
			}
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessCommand(const render::CommandProxy* proxy) {
		const render::CommandHeader* commandHeader = reinterpret_cast<const render::CommandHeader*>(proxy->data);

		switch (commandHeader->commandType) {
		case render::CommandType::RENDER:
		{
			const render::RenderJobCommand* renderCommand = static_cast<const render::RenderJobCommand*>(commandHeader);
			ProcessJobPackage(&renderCommand->jobPackage);
			break;
		}
		case render::CommandType::CLEAR:
		{
			u32 fl = static_cast<const render::ClearCommand*>(commandHeader)->flags;
			f32 color[4] = {0,0,0,1};

			gl::draw::ClearColor(color);
			gl::draw::Clear((fl & render::ClearFlags::COLOR) > 0, (fl & render::ClearFlags::DEPTH) > 0, (fl & render::ClearFlags::STENCIL) > 0);
			break;
		}
		case render::CommandType::RESOURCE_ALLOCATION:
		case render::CommandType::RESOURCE_DEALLOCATION:
		default:
			ASSERT(false);
			break;
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackage(const render::JobPackage* jobPackage) {
		const char* resourceData = reinterpret_cast<const char*>(jobPackage) + jobPackage->resourcesOffset;
		const render::Resource* resource = reinterpret_cast<const render::Resource*>(resourceData);
		const render::Resource* resourceEnd = reinterpret_cast<const render::Resource*>(resourceData + jobPackage->resourcesCount);


		const char* uniforms = reinterpret_cast<const char*>(jobPackage) + jobPackage->uniformsOffset;

		ASSERT((char*) resourceEnd <= (char*) jobPackage + jobPackage->size);

		const render::Resource* shaderResource = &jobPackage->shader;
		ASSERT(render::ResourceGetType(shaderResource) == render::ResourceType::SHADER);

		const u32* index = _handleToIndexMap.Find(shaderResource->handle);
		ASSERT(index);

		const ShaderData& shaderData = _shaders.Get(*index);
		gl::program::Use(shaderData.shader);

		ProcessJobPackageResources(resource, resourceEnd);
		ProcessJobPackageUniforms(uniforms, jobPackage->uniformsCount, shaderData);
		ProcessJobPackageBatch(&jobPackage->batch);
		gl::vertex::UnbindArrayObject();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageResources(const render::Resource* resourceBegin, const render::Resource* resourceEnd) {
		u32 texturesCount = 0;

		// OpenGL specific find VAO first!
		const render::Resource* vertexDescription = nullptr;
		for (const render::Resource* resource = resourceBegin; resource < resourceEnd; ++resource) {
			if (render::ResourceGetType(resource) == render::ResourceType::VERTEX_DESCRIPTION) {
				vertexDescription = resource;
				break;
			}
		}

		const render::Resource* resource = resourceBegin;

		if (vertexDescription) {
			ProcessJobPackageResource(vertexDescription, texturesCount);

			for (; resource < vertexDescription; ++resource)
				ProcessJobPackageResource(resource, texturesCount);

			resource = vertexDescription + 1;
		} // else -> Not using VAO is for now allowed

		for (; resource < resourceEnd; ++resource)
			ProcessJobPackageResource(resource, texturesCount);
	}

	void GLRenderDevice::ProcessJobPackageResource(const render::Resource* resource, u32& inOutTextureCount) {
		const u32* index = _handleToIndexMap.Find(resource->handle);
		ASSERT(index);
		
		switch (render::ResourceGetType(resource)) {
		case render::ResourceType::SHADER: 
			ASSERT(false); 
			break;
		case render::ResourceType::TEXTURE:
		{
			u32 texture = _textures.Get(*index);
			gl::texture::SetActiveTexture(inOutTextureCount++); // @TODO find out if seting every frame is ok
			gl::texture::Bind(texture);
			break;
		}
		case render::ResourceType::VERTEX_BUFFER:
		{
			u32 vb = _vertexBuffers.Get(*index);
			gl::vertex::BindArrayBuffer(vb);
			break;
		}
		case render::ResourceType::VERTEX_DESCRIPTION:
		{
			u32 vb = _vertexDescriptions.Get(*index);
			gl::vertex::BindArrayObject(vb);
			break;
		}
		case render::ResourceType::INDEX_BUFFER:
		{
			u32 ib = _indexBuffers.Get(*index);
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
		const render::UniformHeader* header = static_cast<const render::UniformHeader*>(data);
		
		const UniformData* uniform = shaderData.uniformData.Find(header->nameHash);

		ASSERT(uniform);
		const char* result = static_cast<const char*>(data);

		switch (uniform->type) {
		case render::UniformType::FLOAT:
			gl::uniform::Set1f(uniform->location, reinterpret_cast<const render::FloatUniform*>(header)->value);
			result += sizeof(render::FloatUniform);
			break;
		case render::UniformType::INT:
			gl::uniform::Set1i(uniform->location, reinterpret_cast<const render::IntUniform*>(header)->value);
			result += sizeof(render::IntUniform);
			break;
		case render::UniformType::VECTOR2:
			gl::uniform::Set2f(uniform->location, reinterpret_cast<const render::Vector2Uniform*>(header)->values);
			result += sizeof(render::Vector2Uniform);
			break;
		case render::UniformType::VECTOR3:
			gl::uniform::Set3f(uniform->location, reinterpret_cast<const render::Vector3Uniform*>(header)->values);
			result += sizeof(render::Vector3Uniform);
			break;
		case render::UniformType::MATRIX:
			gl::uniform::SetMatrix4f(uniform->location, reinterpret_cast<const render::MatrixUniform*>(header)->values);
			result += sizeof(render::MatrixUniform);
			break;
		case render::UniformType::SAMPLER2D:
		{
			u32* index = _handleToIndexMap.Find(reinterpret_cast<const render::Sampler2DUniform*>(header)->handle);
			u32 glID = _textures.Get(*index);
			gl::texture::SetActiveTexture(0); // @TODO find out if seting every frame is ok
			gl::texture::Bind(glID);
			result += sizeof(render::Sampler2DUniform);
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		return result;
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageBatch(const render::BatchDescription* batch) {
		switch (batch->type) {
		case render::BatchType::LINES:
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
		case render::BatchType::LINE_LOOP:
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
		case render::BatchType::TRIANGLES:
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
	void GLRenderDevice::AllocateShader(const render::Shader* shader) {
		ASSERT(!_handleToIndexMap.Find(shader->handle));

		u32 vs = gl::util::CompileVertexShader(shader->vertexStage.data);
		u32 fs = gl::util::CompileFragmentShader(shader->fragmentStage.data);
		u32 program = gl::util::CreateProgram(vs, fs);

		gl::shader::Delete(vs);
		gl::shader::Delete(fs);
		
		u32 index;
		{
			ShaderData shaderData;
			shaderData.shader = program;
			shaderData.uniformData.Init(_resourcesAllocator);

			index = _shaders.Add(core::move(shaderData));
			_handleToIndexMap.Add(shader->handle, index);
		}
		

		// @TODO FIX this

		core::HashMap<UniformData>& uniformMap = _shaders.Get(index).uniformData;

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
	void GLRenderDevice::AllocateTexture(const render::Texture* texture) {
		ASSERT(!_handleToIndexMap.Find(texture->handle));

		u32 glID = gl::texture::Create();
		gl::texture::Bind(glID);
		gl::texture::SetParameterMinMagFilter(texture->minFilter, texture->maxFilter);
		gl::texture::SetParameterWrapST(texture->wrapS, texture->wrapT);
		gl::texture::SetData(texture->data, texture->width, texture->height, texture->mipmaps, texture->format);
		gl::texture::Unbind();

		u32 index = _textures.Add(glID);
		_handleToIndexMap.Add(texture->handle, index);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexBuffer(const render::VertexBuffer* vb) {
		ASSERT(!_handleToIndexMap.Find(vb->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, &glID);
		gl::vertex::BindArrayBuffer(glID);

		if (vb->bufferType == render::BufferType::STATIC) {
			gl::vertex::ArrayBufferStaticData(vb->data, vb->dataSize);
		}
		else if (vb->bufferType == render::BufferType::DYNAMIC_UPDATE) {
			gl::vertex::ArrayBufferDynamicData(vb->data, vb->dataSize);
		}

		u32 index = _vertexBuffers.Add(glID);
		_handleToIndexMap.Add(vb->handle, index);

		gl::vertex::UnbindArrayBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexDescription(const render::VertexDescription* vbd) {
		ASSERT(!_handleToIndexMap.Find(vbd->handle));
		// @TODO? current requirement: everything mentioned by vbd has to be already in GPU -> maybe rework

		u32 glID;
		gl::vertex::CreateArrayObjects(1, &glID);
		gl::vertex::BindArrayObject(glID);

		u64 ibh = vbd->indexBufferHandle;
		if (vbd->hasIndexBuffer) {
			u32* index = _handleToIndexMap.Find(vbd->handle);
			u32 ibID = _indexBuffers.Get(*index);
			gl::vertex::BindIndexArrayBuffer(ibID);
		}

		u64 lastVbh = U64MAX;
		u32 vbID;
		for (u32 i = 0; i < vbd->attributesCount; ++i) {
			const render::VertexAttribute* attribute = &vbd->attributes[i];

			u64 vbh = attribute->vertexBufferHandle;
			if (vbh != lastVbh) {
				u32* index = _handleToIndexMap.Find(vbd->handle);
				u32 newVbID = _vertexBuffers.Get(*index);

				vbID = newVbID;
				gl::vertex::BindArrayBuffer(vbID);
			}

			switch (attribute->type) {
			case render::VertexAttributeType::F32: 
				gl::vertex::EnableAttributeArray(attribute->index);
				gl::vertex::AttributePointerFloat(attribute->index, 1, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					gl::vertex::AttributeDivisor(attribute->index, attribute->divisor);
				break;
			case render::VertexAttributeType::V2:
				gl::vertex::EnableAttributeArray(attribute->index);
				gl::vertex::AttributePointerFloat(attribute->index, 2, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					gl::vertex::AttributeDivisor(attribute->index, attribute->divisor);
				break;
			case render::VertexAttributeType::M4:
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
		
		u32 index = _vertexDescriptions.Add(glID);
		_handleToIndexMap.Add(vbd->handle, index);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateIndexBuffer(const render::IndexBuffer* ib) {
		ASSERT(!_handleToIndexMap.Find(ib->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, &glID);
		gl::vertex::BindIndexArrayBuffer(glID);

		if (ib->bufferType == render::BufferType::STATIC) {
			gl::vertex::IndexArrayBufferStaticData(ib->data, ib->dataSize);
		}
		else if (ib->bufferType == render::BufferType::DYNAMIC_UPDATE) {
			gl::vertex::IndexArrayBufferDynamicData(ib->data, ib->dataSize);
		}

		u32 index = _indexBuffers.Add(glID);
		_handleToIndexMap.Add(ib->handle, index);

		gl::vertex::UnbindIndexBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateShader(const render::Resource* shader) {
		u32* index = _handleToIndexMap.Find(shader->handle);
		ASSERT(index);

		if (index) {
			ShaderData& shaderData = _shaders.Get(*index);

			gl::program::Delete(shaderData.shader);

			_handleToIndexMap.Remove(shader->handle);
			_shaders.Remove(*index);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateTexture(const render::Resource* texture) {
		u32* index = _handleToIndexMap.Find(texture->handle);
		ASSERT(index);

		if (index) {
			u32 glID = _textures.Get(*index);
			gl::program::Delete(glID);

			_handleToIndexMap.Remove(texture->handle);
			_textures.Remove(*index);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexBuffer(const render::Resource* vb) {
		u32* index = _handleToIndexMap.Find(vb->handle);
		ASSERT(index);

		if (index) {
			u32 glID = _vertexBuffers.Get(*index);
			gl::vertex::DeleteBuffers(&glID, 1);

			_handleToIndexMap.Remove(vb->handle);
			_vertexBuffers.Remove(*index);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexInformation(const render::Resource* vbi) {
		u32* index = _handleToIndexMap.Find(vbi->handle);
		ASSERT(index);

		if (index) {
			u32 glID = _vertexDescriptions.Get(*index);
			gl::vertex::DeleteArrayObjects(&glID, 1);

			_handleToIndexMap.Remove(vbi->handle);
			_vertexDescriptions.Remove(*index);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateIndexBuffer(const render::Resource* ib) {
		u32* index = _handleToIndexMap.Find(ib->handle);
		ASSERT(index);

		if (index) {
			u32 glID = _indexBuffers.Get(*index);
			gl::vertex::DeleteBuffers(&glID, 1);

			_handleToIndexMap.Remove(ib->handle);
			_indexBuffers.Remove(*index);
		}
	}
}


