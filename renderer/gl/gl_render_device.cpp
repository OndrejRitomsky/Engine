#include "gl_render_device.h"

#include "gl_wrapper.h"

#include <core/common/utility.h>
#include <core/collection/lookuparray.inl>
#include <core/collection/hashmap.inl>

#include "../api/render_context.h"
#include "../api/resource_context.h"

#include "../api/command/job_package.h"
#include "../api/command/uniform.h"

#include "../api/internal/commands.h"

#include "../api/render_resources.h"


// device context
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <extern/include/gl/glew.h>
#include <extern/include/gl/wglew.h>

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
	// @TODO just fixed array
	struct ShaderData {
		u32 shader;
		core::HashMap<UniformData> uniformData;
	};

	GLRenderDevice::GLRenderDevice() :
		_commandBuffer(nullptr),
		_bufferSize(0),
		_bufferCapacity(0),
		_frameAllocator(nullptr),
		_hashFunction(nullptr) {
	}

	GLRenderDevice::~GLRenderDevice() {

		if (_commandBuffer)
			Deallocate(_resourcesAllocator, _commandBuffer);

		ASSERT2(_handleToIndexMap.Count() == 0, "Deallocate commands must be sent!");

		auto it = _handleToIndexMap.CIterator();
		for (u32 i = 0; i < it.count; ++i) {
			const Resource* resource = reinterpret_cast<const Resource*>(&it.keys[i]);
			switch (ResourceGetType(resource)) {
			case ResourceType::SHADER:
			{
				const ShaderData& shaderData = _shaders.Get(it.values[i]);
				GlProgramDelete(shaderData.shader);
				_shaders.Remove(it.values[i]);
				break;
			}
			case ResourceType::TEXTURE:
			{
				u32 index = _textures.Get(it.values[i]);
				GlTextureDelete(index); 
				_textures.Remove(it.values[i]);
				break;
			}
			case ResourceType::VERTEX_BUFFER:
			{
				u32 index = _vertexBuffers.Get(it.values[i]);
				GlBuffersDelete((u32*) &index, 1);
				_vertexBuffers.Remove(it.values[i]);
				break;
			}
			case ResourceType::VERTEX_DESCRIPTION:
			{
				u32 index = _vertexDescriptions.Get(it.values[i]);
				GlVertexArraysDelete((u32*) &index, 1);
				_vertexDescriptions.Remove(it.values[i]);
				break;
			}
			case ResourceType::INDEX_BUFFER: 
			{
				u32 index = _indexBuffers.Get(it.values[i]);
				GlBuffersDelete((u32*) &index, 1);
				_indexBuffers.Remove(it.values[i]);
				break;
			}
			default: ASSERT(false); break;
			}
		}
	}

	void GLRenderDevice::Init(core::IAllocator* resourcesAllocator, core::IAllocator* frameAllocator, HashFunction hashFunction, void* deviceContext) {
		_frameAllocator = frameAllocator;
		_resourcesAllocator = resourcesAllocator;

		_hashFunction = hashFunction;

		_shaders.Init(resourcesAllocator, 128);
		_textures.Init(resourcesAllocator, 128);
		_vertexDescriptions.Init(resourcesAllocator, 128);
		_vertexBuffers.Init(resourcesAllocator, 128);
		_indexBuffers.Init(resourcesAllocator, 128);

		_handleToIndexMap.Init(resourcesAllocator);

		bool ok = GLInit(deviceContext);
		ASSERT(ok);
	}

	bool GLRenderDevice::GLInit(void* deviceContext) {
		static_assert(sizeof(HDC) == sizeof(void*), "Size missmatch");
		static_assert(sizeof(HGLRC) == sizeof(void*), "Size missmatch");

		HDC hdc = (HDC) deviceContext;

		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

		if (nPixelFormat == 0)
			return false;

		BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

		if (!bResult)
			return false;

		HGLRC tempContext = wglCreateContext(hdc);
		wglMakeCurrent(hdc, tempContext);

		GLenum err = glewInit();
		if (GLEW_OK != err)
			return false;

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

		HGLRC rdc;

		if (wglewIsSupported("WGL_ARB_create_context") == 1) {
			rdc = wglCreateContextAttribsARB(hdc, 0, attribs);
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(tempContext);
			wglMakeCurrent(hdc, rdc);
		}
		else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
			rdc = tempContext;
		}

		//Checking GL version
		const GLubyte *GLVersionString = glGetString(GL_VERSION);

		//Or better yet, use the GL3 way to get the version number
		int OpenGLVersion[2];
		glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

		if (!rdc)
			return false;

		_renderDeviceContext = rdc;
		_deviceContext = hdc;

		GlViewPort(1024, 768);

		return true;
	}


	void GLRenderDevice::Render(const RenderContext* rc, u64 count) {
		if (!_bufferCapacity) {
			// @TODO allocators should have out capacity
			_bufferCapacity = 1024 * 1024;

			_commandBuffer = static_cast<char*>(Allocate(_resourcesAllocator, _bufferCapacity, alignof(char)));
		}

		for (u64 i = 0; i < count; ++i) {
			u32 commandsCount;
			const CommandProxy* proxies = rc[i].GetCommands(commandsCount);

			u64 commandsSize = commandsCount * sizeof(CommandProxy);
			if (_bufferSize + commandsSize > _bufferCapacity) {
				_bufferCapacity *= 2;

				char* result = static_cast<char*>(Allocate(_resourcesAllocator, _bufferCapacity, alignof(char)));
				core::Memcpy(result, _commandBuffer, _bufferSize);
				Deallocate(_resourcesAllocator, _commandBuffer);
				_commandBuffer = result;
			}

			core::Memcpy(_commandBuffer + _bufferSize, proxies, commandsSize);
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

	void GLRenderDevice::ManageResources(const ResourceContext* rrc, u64 count) {
		const ResourceContext* resourceContext = rrc;
		const ResourceContext* resourceContextEnd = rrc + count;

		for (; resourceContext < resourceContextEnd; resourceContext++) {

			if (resourceContext->type == ResourceCommandType::RESOURCE_ALLOCATION) {

				switch (ResourceGetType(resourceContext->resource)) {
				case ResourceType::SHADER: AllocateShader((const Shader*)(resourceContext->resource)); break;
				case ResourceType::TEXTURE:	AllocateTexture((const Texture*)(resourceContext->resource)); break;
				case ResourceType::VERTEX_BUFFER: AllocateVertexBuffer((const VertexBuffer*)(resourceContext->resource)); break;
				case ResourceType::VERTEX_DESCRIPTION: AllocateVertexDescription((const VertexDescription*)(resourceContext->resource)); break;
				case ResourceType::INDEX_BUFFER: AllocateIndexBuffer((const IndexBuffer*)(resourceContext->resource)); break;
				default: ASSERT(false); break;
				}

			}
			else if (resourceContext->type == ResourceCommandType::RESOURCE_DEALLOCATION) {

				switch (ResourceGetType(resourceContext->resource)) {
				case ResourceType::SHADER:             DeallocateShader(resourceContext->resource); break;
				case ResourceType::TEXTURE:            DeallocateTexture(resourceContext->resource); break;
				case ResourceType::VERTEX_BUFFER:      DeallocateVertexBuffer(resourceContext->resource); break;
				case ResourceType::VERTEX_DESCRIPTION: DeallocateVertexInformation(resourceContext->resource); break;
				case ResourceType::INDEX_BUFFER:       DeallocateIndexBuffer(resourceContext->resource); break;
				default: ASSERT(false); break;
				}

			}
			else {
				ASSERT(false);
			}
		}
	}

	void GLRenderDevice::ProcessCommand(const CommandProxy* proxy) {
		const CommandHeader* commandHeader = reinterpret_cast<const CommandHeader*>(proxy->data);

		switch (commandHeader->commandType) {
		case CommandType::VIEWPORT:
		{
			const ViewPortCommand* vpCommand = (const ViewPortCommand*) commandHeader;
			GlViewPort(vpCommand->width, vpCommand->height);
			break;
		}
		case CommandType::RENDER:
		{
			const RenderJobCommand* renderCommand = (const RenderJobCommand*) commandHeader;
			ProcessJobPackage(&renderCommand->jobPackage);
			break;
		}
		case CommandType::CLEAR:
		{
			u32 fl = static_cast<const ClearCommand*>(commandHeader)->flags;
			f32 color[4] = {0,0,0,1};

			GlClearColor(color);
			GlClear((fl & ClearFlags::COLOR) > 0, (fl & ClearFlags::DEPTH) > 0, (fl & ClearFlags::STENCIL) > 0);
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

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
		GlProgramUse(shaderData.shader);

		ProcessJobPackageResources(resource, resourceEnd);
		ProcessJobPackageUniforms(uniforms, jobPackage->uniformsCount, shaderData);
		ProcessJobPackageBatch(&jobPackage->batch);
		GlVertexArrayUnbind();
	}

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
			GlTextureSetActiveTexture(inOutTextureCount++); // @TODO find out if seting every frame is ok
			GlTextureBind(texture);
			break;
		}
		case ResourceType::VERTEX_BUFFER:
		{
			u32 vb = _vertexBuffers.Get(*handle);
			GlArrayBufferBind(vb);
			break;
		}
		case ResourceType::VERTEX_DESCRIPTION:
		{
			u32 vb = _vertexDescriptions.Get(*handle);
			GlVertexArrayBind(vb);
			break;
		}
		case ResourceType::INDEX_BUFFER:
		{
			u32 ib = _indexBuffers.Get(*handle);
			GlIndexArrayBufferBind(ib);
			break;
		}
		default: ASSERT(false); break;
		}
	}

	void GLRenderDevice::ProcessJobPackageUniforms(const void* data, u32 count, const ShaderData& shaderData) {
		const void* current = data;
		u32 i = 0;
		while (i++ < count)
			current = ProcessJobPackageUniform(current, shaderData);
	}

	const void* GLRenderDevice::ProcessJobPackageUniform(const void* data, const ShaderData& shaderData) {
		const UniformHeader* header = static_cast<const UniformHeader*>(data);
		
		const UniformData* uniform = shaderData.uniformData.Find(header->nameHash);

		ASSERT(uniform);
		const char* result = static_cast<const char*>(data);

		switch (uniform->type) {
		case UniformType::FLOAT:
			GlUniformSet1f(uniform->location, reinterpret_cast<const FloatUniform*>(header)->value);
			result += sizeof(FloatUniform);
			break;
		case UniformType::INT:
			GlUniformSet1i(uniform->location, reinterpret_cast<const IntUniform*>(header)->value);
			result += sizeof(IntUniform);
			break;
		case UniformType::VECTOR2:
			ASSERT(false); // alignment :((
			GlUniformSet2f(uniform->location, reinterpret_cast<const Vector2Uniform*>(header)->values);
			result += sizeof(Vector2Uniform);
			break;
		case UniformType::VECTOR3:
			GlUniformSet3f(uniform->location, reinterpret_cast<const Vector3Uniform*>(header)->values);
			result += sizeof(Vector3Uniform);
			break;
		case UniformType::MATRIX:
		{
			float values[16];
			core::Memcpy(values, reinterpret_cast<const MatrixUniform*>(header)->values, sizeof(values));
			GlUniformSetMatrix4f(uniform->location, values);
			result += sizeof(MatrixUniform);
			break;
		}
		case UniformType::SAMPLER2D:
		{
			const Sampler2DUniform* uniform = reinterpret_cast<const Sampler2DUniform*>(header);
			core::Handle* handle = _handleToIndexMap.Find(uniform->handle);

			u32 glID = _textures.Get(*handle);
			GlTextureSetActiveTexture(0); // @TODO find out if seting every frame is ok
			GlTextureBind(glID);
			result += sizeof(Sampler2DUniform);
			break;
		}
		default:
			ASSERT(false);
			break;
		}

		return result;
	}

	void GLRenderDevice::ProcessJobPackageBatch(const BatchDescription* batch) {
		switch (batch->type) {
		case BatchType::LINES:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					GlDrawLineElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // LineArrayInstanced
			}
			else {
				if (batch->indices > 0)
					GlDrawLineElements(batch->indices, batch->indexType);
				else
					GlDrawLineArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		case BatchType::LINE_LOOP:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					GlDrawLineLoopElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // LineLoopArrayInstanced
			}
			else {
				if (batch->indices > 0)
					GlDrawLineLoopElements(batch->indices, batch->indexType);
				else
					GlDrawLineLoopArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		case BatchType::TRIANGLES:
		{
			if (batch->instances > 1) {
				if (batch->indices > 0)
					GlDrawTriangleElementsInstanced(batch->indices, batch->indexType, batch->instances);
				else
					ASSERT(false); // TriangleArrayInstanced
			}
			else {
				if (batch->indices > 0)
					GlDrawTriangleElements(batch->indices, batch->indexType);
				else
					GlDrawTriangleArrays(batch->vertexOffset, batch->vertices);
			}
			break;
		}
		}
	}

	void GLRenderDevice::AllocateShader(const Shader* shader) {
		ASSERT(!_handleToIndexMap.Find(shader->handle));

		u32 vs = GlShaderVertex();
		u32 fs = GlShaderFragment();
		GlShaderCompile(vs, &shader->vertexStage.data, 1);
		GlShaderCompile(fs, &shader->fragmentStage.data, 1);

		u32 program = GlProgram(vs, fs);

		GlShaderDelete(vs);
		GlShaderDelete(fs);
		
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

		i32 count = GlUniformCount(program);
		for (i32 i = 0; i < count; ++i) {
			i32 length;
			char buffer[64];

			UniformData data;
			GlUniformInfo(program, i, buffer, 64, length, data.size, data.glUniformType);

			data.location = GlUniformLocation(program, buffer);
			data.type = GlTypeToUniformType(data.glUniformType);
			uniformMap.Add(_hashFunction(buffer, length), core::move(data));
		}
	}

	void GLRenderDevice::AllocateTexture(const Texture* texture) {
		ASSERT(!_handleToIndexMap.Find(texture->handle));

		u32 glID = GlTexture();
		GlTextureBind(glID);
		GlTextureSetParameterMinMagFilter(texture->minFilter, texture->maxFilter);
		GlTextureSetParameterWrapST(texture->wrapS, texture->wrapT);
		GlTextureSetData(texture->data, texture->width, texture->height, texture->mipmaps, texture->format);
		GlTextureUnbind();

		core::Handle handle = _textures.Add(glID);
		_handleToIndexMap.Add(texture->handle, handle);
	}

	void GLRenderDevice::AllocateVertexBuffer(const VertexBuffer* vb) {
		ASSERT(!_handleToIndexMap.Find(vb->handle));

		u32 glID;
		GlBuffers(1, (u32*) &glID);
		GlArrayBufferBind(glID);

		if (vb->bufferType == BufferType::STATIC) {
			GlArrayBufferStaticData(vb->data, vb->dataSize);
		}
		else if (vb->bufferType == BufferType::DYNAMIC_UPDATE) {
			GlArrayBufferDynamicData(vb->data, vb->dataSize);
		}

		core::Handle handle = _vertexBuffers.Add(glID);
		_handleToIndexMap.Add(vb->handle, handle);

		GlArrayBufferUnbind();
	}

	void GLRenderDevice::AllocateVertexDescription(const VertexDescription* vbd) {
		ASSERT(!_handleToIndexMap.Find(vbd->handle));
		// @TODO? current requirement: everything mentioned by vbd has to be already in GPU -> maybe rework

		u32 glID;
		GlVertexArrays(1, &glID);
		GlVertexArrayBind(glID);

		if (vbd->hasIndexBuffer) {
			core::Handle* handle = _handleToIndexMap.Find(vbd->indexBufferHandle);
			u32 ibID = _indexBuffers.Get(*handle);
			GlIndexArrayBufferBind(ibID);
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
				GlArrayBufferBind(vbID);
			}

			switch (attribute->type) {
			case VertexAttributeType::F32: 
				GlEnableAttributeArray(attribute->index);
				GlAttributePointerFloat(attribute->index, 1, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					GlAttributeDivisor(attribute->index, attribute->divisor);
				break;
			case VertexAttributeType::V2:
				GlEnableAttributeArray(attribute->index);
				GlAttributePointerFloat(attribute->index, 2, attribute->stride, attribute->offset, attribute->normalize);
				if (attribute->divisor > 0)
					GlAttributeDivisor(attribute->index, attribute->divisor);
				break;
			case VertexAttributeType::M4:
				for (u32 i = 0; i < 4; ++i) {
					u32 index = attribute->index + i;
					GlEnableAttributeArray(index + i);
					GlAttributePointerFloat(index + i, 4, attribute->stride, attribute->offset + 4 * sizeof(f32) * i, attribute->normalize);
					if (attribute->divisor > 0)
						GlAttributeDivisor(index, attribute->divisor);
				}
				break;
			default: 
				ASSERT(false);
				break;
			}
		}

		//gl::vertex::UnbindIndexBuffer();
		//gl::vertex::UnbindArrayBuffer();
		GlVertexArrayUnbind();
		
		_handleToIndexMap.Add(vbd->handle, _vertexDescriptions.Add(glID));
	}

	void GLRenderDevice::AllocateIndexBuffer(const IndexBuffer* ib) {
		ASSERT(!_handleToIndexMap.Find(ib->handle));

		u32 glID;
		GlBuffers(1, (u32*) &glID);
		GlIndexArrayBufferBind(glID);

		if (ib->bufferType == BufferType::STATIC) {
			GlIndexArrayBufferStaticData(ib->data, ib->dataSize);
		}
		else if (ib->bufferType == BufferType::DYNAMIC_UPDATE) {
			GlIndexArrayBufferDynamicData(ib->data, ib->dataSize);
		}

		_handleToIndexMap.Add(ib->handle, _indexBuffers.Add(glID));

		GlIndexBufferUnbind();
	}

	void GLRenderDevice::DeallocateShader(const Resource* shader) {
		core::Handle* handle = _handleToIndexMap.Find(shader->handle);
		ASSERT(handle);

		if (handle) {
			ShaderData& shaderData = _shaders.Get(*handle);

			GlProgramDelete(shaderData.shader);

			_handleToIndexMap.SwapRemove(shader->handle);
			_shaders.Remove(*handle);
		}
	}

	void GLRenderDevice::DeallocateTexture(const Resource* texture) {
		core::Handle* handle = _handleToIndexMap.Find(texture->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _textures.Get(*handle);
			GlProgramDelete(glID);

			_handleToIndexMap.SwapRemove(texture->handle);
			_textures.Remove(*handle);
		}
	}

	void GLRenderDevice::DeallocateVertexBuffer(const Resource* vb) {
		core::Handle* handle = _handleToIndexMap.Find(vb->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _vertexBuffers.Get(*handle);
			GlBuffersDelete(&glID, 1);

			_handleToIndexMap.SwapRemove(vb->handle);
			_vertexBuffers.Remove(*handle);
		}
	}

	void GLRenderDevice::DeallocateVertexInformation(const Resource* vbi) {
		core::Handle* handle = _handleToIndexMap.Find(vbi->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _vertexDescriptions.Get(*handle);
			GlVertexArraysDelete(&glID, 1);

			_handleToIndexMap.SwapRemove(vbi->handle);
			_vertexDescriptions.Remove(*handle);
		}
	}

	void GLRenderDevice::DeallocateIndexBuffer(const Resource* ib) {
		core::Handle* handle = _handleToIndexMap.Find(ib->handle);
		ASSERT(handle);

		if (handle) {
			u32 glID = _indexBuffers.Get(*handle);
			GlBuffersDelete(&glID, 1);

			_handleToIndexMap.SwapRemove(ib->handle);
			_indexBuffers.Remove(*handle);
		}
	}
}


