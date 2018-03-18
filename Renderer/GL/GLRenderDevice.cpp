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

	struct ShaderReflection {
		core::HashMap<UniformData> uniformData; // This could be from pool allocator
	};

	struct ShaderData {
		u32 shader;
		u32 reflectionIndex; // This indirection exists, so hashmaps arent copied around if map with shaders is changing
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

		ASSERT2(_shaders.Count() == 0, "Deallocate commands must be sent!");
		ASSERT2(_textures.Count() == 0, "Deallocate commands must be sent!");
		ASSERT2(_vertexDescriptions.Count() == 0, "Deallocate commands must be sent!");
		ASSERT2(_vertexBuffers.Count() == 0, "Deallocate commands must be sent!");
		ASSERT2(_indexBuffers.Count() == 0, "Deallocate commands must be sent!");

		// THIS CALLS MIGHT ASSERT, it might be too late deleting device, thats why the^ asserts
		for (auto& value : _shaders) {
			gl::program::Delete(value.shader);
			_shaderReflection.Remove(value.reflectionIndex);
		}

		for (auto& value : _textures)
			gl::texture::Delete(value);

		for (auto& value : _vertexDescriptions)
			gl::vertex::DeleteArrayObjects(&value, 1);

		for (auto& value : _vertexBuffers)
			gl::vertex::DeleteBuffers(&value, 1);

		for (auto& value : _indexBuffers)
			gl::vertex::DeleteBuffers(&value, 1);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction) {
		_frameAllocator = frameAllocator;
		_resourcesAllocator = resourcesAllocator;

		_hashFunction = hashFunction;
		_shaderReflection.Init(_resourcesAllocator, 1024);

		_shaders.Init(resourcesAllocator);
		_textures.Init(resourcesAllocator);
		_vertexDescriptions.Init(resourcesAllocator);
		_vertexBuffers.Init(resourcesAllocator);
		_indexBuffers.Init(resourcesAllocator);
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

		// @TODO
		// SORT COMMANDS!!!!
		// MAYBE render pipeline will do this

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

				switch (resource->resourceType) {
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

				switch (resource->resourceType) {
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
		ASSERT(shaderResource->resourceType == render::ResourceType::SHADER);


		ShaderData* shaderData = _shaders.Find(shaderResource->handle);
		ASSERT(shaderData);
		gl::program::Use(shaderData->shader);

		ProcessJobPackageResources(resource, resourceEnd);
		ProcessJobPackageUniforms(uniforms, jobPackage->uniformsCount, shaderData->reflectionIndex);
		ProcessJobPackageBatch(&jobPackage->batch);
		gl::vertex::UnbindArrayObject();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageResources(const render::Resource* resourceBegin, const render::Resource* resourceEnd) {
		u32 texturesCount = 0;

		// OpenGL specific find VAO first!
		const render::Resource* vertexDescription = nullptr;
		for (const render::Resource* resource = resourceBegin; resource < resourceEnd; ++resource) {
			if (resource->resourceType == render::ResourceType::VERTEX_DESCRIPTION) {
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
		switch (resource->resourceType) {
		case render::ResourceType::SHADER: 
			ASSERT(false); 
			break;
		case render::ResourceType::TEXTURE:
		{
			u32* texture = _textures.Find(resource->handle);
			ASSERT(texture);
			gl::texture::SetActiveTexture(inOutTextureCount++); // @TODO find out if seting every frame is ok
			gl::texture::Bind(*texture);
			break;
		}
		case render::ResourceType::VERTEX_BUFFER:
		{
			u32* vb = _vertexBuffers.Find(resource->handle);
			ASSERT(vb);
			gl::vertex::BindArrayBuffer(*vb);
			break;
		}
		case render::ResourceType::VERTEX_DESCRIPTION:
		{
			u32* vb = _vertexDescriptions.Find(resource->handle);
			ASSERT(vb);
			gl::vertex::BindArrayObject(*vb);
			break;
		}
		case render::ResourceType::INDEX_BUFFER:
		{
			u32* ib = _indexBuffers.Find(resource->handle);
			ASSERT(ib);
			gl::vertex::BindIndexArrayBuffer(*ib);
			break;
		}
		default: ASSERT(false); break;
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::ProcessJobPackageUniforms(const void* data, u32 count, u32 reflectionIndex) {
		const void* current = data;
		u32 i = 0;

		ShaderReflection& ref = _shaderReflection.Get(reflectionIndex);

		while (i++ < count)
			current = ProcessJobPackageUniform(current, &ref);
	}

	//---------------------------------------------------------------------------
	const void* GLRenderDevice::ProcessJobPackageUniform(const void* data, ShaderReflection* reflection) {
		const render::UniformHeader* header = static_cast<const render::UniformHeader*>(data);
		
		const UniformData* uniform = reflection->uniformData.Find(header->nameHash);

		ASSERT(uniform);
		const char* result = static_cast<const char*>(data);

		switch (uniform->type) {
		case render::UniformType::FLOAT:
			gl::uniform::Set1f(uniform->location, ((render::FloatUniform*) header)->value);
			result += sizeof(render::FloatUniform);
			break;
		case render::UniformType::INT:
			gl::uniform::Set1i(uniform->location, ((render::IntUniform*) header)->value);
			result += sizeof(render::IntUniform);
			break;
		case render::UniformType::VECTOR2:
			gl::uniform::Set2f(uniform->location, ((render::Vector2Uniform*) header)->values);
			result += sizeof(render::Vector2Uniform);
			break;
		case render::UniformType::VECTOR3:
			gl::uniform::Set3f(uniform->location, ((render::Vector3Uniform*) header)->values);
			result += sizeof(render::Vector3Uniform);
			break;
		case render::UniformType::MATRIX:
			gl::uniform::SetMatrix4f(uniform->location, ((render::MatrixUniform*) header)->values);
			result += sizeof(render::MatrixUniform);
			break;
		case render::UniformType::SAMPLER2D:
		{
			u32* glID = _textures.Find(((render::Sampler2DUniform*) header)->handle);
			ASSERT(glID);
			gl::texture::SetActiveTexture(0); // @TODO find out if seting every frame is ok
			gl::texture::Bind(*glID);
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
		ASSERT(!_shaders.Find(shader->handle));

		u32 vs = gl::util::CompileVertexShader(shader->vertexStage.data);
		u32 fs = gl::util::CompileFragmentShader(shader->fragmentStage.data);
		u32 program = gl::util::CreateProgram(vs, fs);

		gl::shader::Delete(vs);
		gl::shader::Delete(fs);

		ShaderReflection reflection;
		reflection.uniformData.Init(_resourcesAllocator);

		ShaderData shaderData;
		shaderData.shader = program;
		shaderData.reflectionIndex = _shaderReflection.Add(core::move(reflection));

		_shaders.Add(shader->handle, shaderData);

		// @TODO FIX this

		core::HashMap<UniformData>& uniformMap = _shaderReflection.Get(shaderData.reflectionIndex).uniformData;

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
		ASSERT(!_textures.Find(texture->handle));

		u32 glID = gl::texture::Create();
		gl::texture::Bind(glID);
		gl::texture::SetParameterMinMagFilter(texture->minFilter, texture->maxFilter);
		gl::texture::SetParameterWrapST(texture->wrapS, texture->wrapT);
		gl::texture::SetData(texture->data, texture->width, texture->height, texture->mipmaps, texture->format);
		gl::texture::Unbind();

		_textures.Add(texture->handle, glID);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexBuffer(const render::VertexBuffer* vb) {
		ASSERT(!_vertexBuffers.Find(vb->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, &glID);
		gl::vertex::BindArrayBuffer(glID);

		if (vb->bufferType == render::BufferType::STATIC) {
			gl::vertex::ArrayBufferStaticData(vb->data, vb->dataSize);
		}
		else if (vb->bufferType == render::BufferType::DYNAMIC_UPDATE) {
			gl::vertex::ArrayBufferDynamicData(vb->data, vb->dataSize);
		}

		_vertexBuffers.Add(vb->handle, glID);

		gl::vertex::UnbindArrayBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateVertexDescription(const render::VertexDescription* vbd) {
		ASSERT(!_vertexDescriptions.Find(vbd->handle));

		// @TODO? current requirement: everything mentioned by vbd has to be already in GPU -> maybe rework

		u32 glID;
		gl::vertex::CreateArrayObjects(1, &glID);
		gl::vertex::BindArrayObject(glID);

		u32 ibh = vbd->indexBufferHandle;
		u32* pIbID = nullptr;
		if (ibh != U32MAX) {
			pIbID = _indexBuffers.Find(ibh);
			ASSERT(pIbID);
			gl::vertex::BindIndexArrayBuffer(*pIbID);
		}

		u32 lastVbh = U32MAX;
		u32 vbID;
		for (u32 i = 0; i < vbd->attributesCount; ++i) {
			const render::VertexAttribute* attribute = &vbd->attributes[i];

			u32 vbh = attribute->vertexBufferHandle;
			if (vbh != lastVbh) {
				u32* pVbID = _vertexBuffers.Find(vbh);
				ASSERT(pVbID);
				vbID = *pVbID;
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
		_vertexDescriptions.Add(vbd->handle, glID);
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::AllocateIndexBuffer(const render::IndexBuffer* ib) {
		ASSERT(!_indexBuffers.Find(ib->handle));

		u32 glID;
		gl::vertex::CreateBuffers(1, &glID);
		gl::vertex::BindIndexArrayBuffer(glID);

		if (ib->bufferType == render::BufferType::STATIC) {
			gl::vertex::IndexArrayBufferStaticData(ib->data, ib->dataSize);
		}
		else if (ib->bufferType == render::BufferType::DYNAMIC_UPDATE) {
			gl::vertex::IndexArrayBufferDynamicData(ib->data, ib->dataSize);
		}

		_indexBuffers.Add(ib->handle, glID);

		gl::vertex::UnbindIndexBuffer();
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateShader(const render::Resource* shader) {
		ShaderData* shaderData = _shaders.Find(shader->handle);
		ASSERT(shaderData);

		if (shaderData) {
			_shaderReflection.Remove(shaderData->reflectionIndex);

			gl::program::Delete(shaderData->shader);
			_shaders.Remove(shader->handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateTexture(const render::Resource* texture) {
		u32* glID = _textures.Find(texture->handle);
		ASSERT(glID);

		if (glID) {
			gl::texture::Delete(*glID);
			_textures.Remove(texture->handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexBuffer(const render::Resource* vb) {
		u32* glID = _vertexBuffers.Find(vb->handle);
		ASSERT(glID);

		if (glID) {
			gl::vertex::DeleteBuffers(glID, 1);
			_vertexBuffers.Remove(vb->handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateVertexInformation(const render::Resource* vbi) {
		u32* glID = _vertexDescriptions.Find(vbi->handle);
		ASSERT(glID);

		if (glID) {
			gl::vertex::DeleteArrayObjects(glID, 1);
			_vertexDescriptions.Remove(vbi->handle);
		}
	}

	//---------------------------------------------------------------------------
	void GLRenderDevice::DeallocateIndexBuffer(const render::Resource* ib) {
		u32* glID = _indexBuffers.Find(ib->handle);
		ASSERT(glID);

		if (glID) {
			gl::vertex::DeleteBuffers(glID, 1);
			_indexBuffers.Remove(ib->handle);
		}
	}
}


