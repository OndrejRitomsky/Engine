#include "render_pipeline.h"

// @WELP 

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/collection/array.inl>
#include <core/allocator/allocators.h>
#include <core/algorithm/hash.h>

#include <core/math/m44.h>

#include <renderer/api/render_resources.h>

#include "../manager/render_object_component_manager.h"

#include "../render_module.h"

#include "../../engine.h"
#include "../../world/entity.h"
#include "../../world/world_module.h"
#include "../../world/manager/transform2d_component_manager.h"

#include "../../frame/frame.h"

#include "../../memory/memory_module.h"
#include "../../memory/permanent_allocator.h"


#include "../../resources/resource_module.h"
#include "../../resources/resource_manager/material_manager.h"
#include "../../resources/resource_manager/mesh_manager.h"
#include "../../resources/resource_manager/texture_manager.h"
#include "../../resources/resource_manager/render_object_manager.h"
#include "../../resources/resource_manager/shader_program_manager.h"
#include "../../resources/resource_manager/shader_stage_manager.h"

#include "../../resources/resources.h"

#include "../../util/handle_manager.h"

#include "../../../renderer/api/render_context.h"
#include "../../../renderer/api/resource_context.h"
#include "../../../renderer/api/render_context_stream.h"
#include "../../../renderer/api/command/job_package.h"
#include "../../../renderer/api/command/uniform.h"

#include "../../../renderer/api/irender_device.h"

namespace eng {

	static_assert(MESH_INDEX_BYTE_SIZE == 2, "Index size missmatch");

	// HANDLES == render::Resource::handle

	struct PipelineMesh {
		u64 vertexDescriptionHandle;
		u64 vertexBufferHandle;
		u64 indexBufferHandle;

		// this shouldnt be here... jobpackage is asking for rendering info and not mesh info! 
		u32 verticesCount;
		u32 indicesCount;
	};

	struct PipelineShader {
		u64 handle;
	};

	struct PipelineTexture {
		u64 handle;
	};

	struct PipelineObject {
		static const u32 MAX_RESOURCES_COUNT = 8;

		Entity entity;

		bool ready;
		u32 vertices;
		u32 indices;
		u64 shaderHandle;

		u32 resourcesCount;
		render::Resource resources[MAX_RESOURCES_COUNT];

		u32 texturesCount; // @TODO 
		render::Resource textures[MATERIAL_TEXTURES_CAPACITY];
	};


	RenderPipeline::RenderPipeline() {

	}

	RenderPipeline::~RenderPipeline() {
		_handleManager->~HandleManager();
	}

	void RenderPipeline::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		_handleManager = PermanentNew(permanentAllocator, HandleManager)();
	}

	void RenderPipeline::Init(Engine* engine) {
		_resourceModule = engine->resourceModule;
		_worldModule = engine->worldModule;

		core::IAllocator* allocator = engine->memoryModule->Allocator();

		_handleManager->Init(allocator);
		_shaders.Init(allocator);
		_textures.Init(allocator);
		_meshes.Init(allocator);
		_objects.Init(allocator);
		_newResources.Init(allocator);

		_allocator = allocator;

		_renderObjectComponentManager = engine->renderModule->GetRenderObjectComponentManager();
	}

	void RenderPipeline::SetViewPort(i32 width, i32 height) {
		_width = width;
		_height = height;
		_viewportChanged = true;
	}

	void RenderPipeline::Render(render::IRenderDevice* renderDevice, const Frame* frame, const RenderObjectComponent* components, u32 componentsCount) {
		if (componentsCount == 0)
			return;

		_newResources.Clear(); // @TODO tag
		const RenderObjectComponent* component = components;
		const RenderObjectComponent* componentEnd = component + componentsCount;
		const RenderObjectManager* renderObjectManager = _resourceModule->GetRenderObjectManager();

		core::SingleTagProxyAllocator renderAllocator;
		SingleTagProxyAllocatorInit(&renderAllocator, frame->tagAllocator, frame->renderPipelineTag);

		// storage for data... tag allocated
		render::RenderContextStream stream;
		stream.Init(&renderAllocator);

		// @TODO figure out render package, how will the data for rendering be stored, before we send it to render context
		// @TODO allocators? reuse render context ? 
		// @TODO lifetime rendercontext have to exist for renderer.. maybe it uses only getcommands?

		render::RenderContext renderContexts[2];
		renderContexts[0].Init(&stream, _allocator);
		renderContexts[0].BeginStateCommands(0);
		renderContexts[0].AddClearCommand(render::ClearFlags::COLOR);
		if (_viewportChanged) {
			_viewportChanged = false;
			renderContexts[0].AddViewPortCommand(_width, _height);
		}

		renderContexts[0].EndStateCommands();

		f32 width = (f32) _width;
		f32 height = (f32) _height;


		renderContexts[1].Init(&stream, _allocator);

		_viewMatrix = MatrixSymetricOrthogonal(width, height, 0.0f, 100.0f);

		u32 drawCommands = 0;

		for (; component < componentEnd; component++) {
			eng::Resource renderObjectResource = _renderObjectComponentManager->GetResource(*component);

			PipelineObject* pipelineObject = _objects.Find((h64) renderObjectResource);

			if (!pipelineObject) {
				const RenderObject& renderObject = renderObjectManager->GetRenderObject(renderObjectResource);

				PipelineObject newPipelineObject;
				CreatePipelineObject(frame, &stream, &renderObject, &newPipelineObject);

				_objects.Add((h64) renderObjectResource, newPipelineObject); // @TODO hashmap return on add
			}
			else {
				auto entity = _renderObjectComponentManager->GetEntity(*component);

				RenderPipelineObject(renderContexts + 1, pipelineObject, &entity);
				drawCommands++;
			}
		}

		if (_newResources.Count() > 0) {
			renderDevice->ManageResources(_newResources.begin(), _newResources.Count());
		}

		renderDevice->Render(renderContexts, drawCommands > 0 ? 2 : 1);


	}

	void RenderPipeline::RenderPipelineObject(render::RenderContext* context, const PipelineObject* pipelineObject, const Entity* entity) {
		Transform2DComponentManager* transforms = _worldModule->GetTransform2DComponentManager();
		auto transformHandle = transforms->Find(*entity);
		M44 transform = transforms->Transform(transformHandle);


		render::BatchDescription batch;
		batch.instances = 1;
		batch.vertices = pipelineObject->vertices;
		batch.indices = pipelineObject->indices;
		batch.type = render::BatchType::TRIANGLES;
		batch.indexType = render::IndexType::U16; // -> actualy device could hold this

		render::JobPackage jobPackage;
		jobPackage.batch = batch;
		jobPackage.shader = {pipelineObject->shaderHandle};

		jobPackage.size = sizeof(render::JobPackage) + pipelineObject->resourcesCount * sizeof(render::Resource)
			+ sizeof(render::MatrixUniform);

		if (pipelineObject->texturesCount == 1) {
			jobPackage.size += sizeof(render::Sampler2DUniform);
		}

		jobPackage.resourcesOffset = sizeof(render::JobPackage);
		jobPackage.resourcesCount = pipelineObject->resourcesCount;
		jobPackage.uniformsOffset = jobPackage.resourcesOffset + pipelineObject->resourcesCount * sizeof(render::Resource);
		jobPackage.uniformsCount = 1 + pipelineObject->texturesCount;
		jobPackage.sortKey = 1;

		render::JobPackage* rjp = context->AddRenderJobPackage(&jobPackage);
		render::Resource* resource = (render::Resource*) core::PointerAdd(rjp, rjp->resourcesOffset);

		for (u32 j = 0; j < pipelineObject->resourcesCount; ++j) {
			resource[j] = pipelineObject->resources[j];
		}

		char* uniforms = (char*) core::PointerAdd(rjp, rjp->uniformsOffset);

		render::MatrixUniform uniform;

		

		//M44 mvp = MatrixScale(100, 100, 1) * MatrixTranslate(0, 0, 1) * _viewMatrix;
		M44 mvp = transform * _viewMatrix;

		uniform.nameHash = core::Hash("mvp", 4);
		core::Memcpy(uniform.values, mvp.data, sizeof(mvp));
		core::Memcpy(uniforms, &uniform, sizeof(render::MatrixUniform));

		uniforms += sizeof(render::MatrixUniform);

		if (pipelineObject->texturesCount == 1) {
			render::Sampler2DUniform textureUniform;
			textureUniform.handle = pipelineObject->textures[0].handle;
			textureUniform.nameHash = core::Hash("diffuseTexture", 15);
			core::Memcpy(uniforms, &textureUniform, sizeof(render::Sampler2DUniform));
		}	
	}


	void RenderPipeline::CreatePipelineObject(const Frame* frame, render::RenderContextStream* stream, const RenderObject* renderObject, PipelineObject* outPipelineObject) {

		const MaterialManager* materialManager = _resourceModule->GetMaterialManager();
		const MeshManager* meshManager = _resourceModule->GetMeshManager();
		const ShaderProgramManager* shaderProgramManager = _resourceModule->GetShaderProgramManager();
		const ShaderStageManager* shaderStageManager = _resourceModule->GetShaderStageManager();
		const TextureManager* textureManager = _resourceModule->GetTextureManager();

		const Material& material = materialManager->GetMaterial(renderObject->material);

		outPipelineObject->texturesCount = 0;
		outPipelineObject->resourcesCount = 0;

		ASSERT(material.texturesCount <= MATERIAL_TEXTURES_CAPACITY);

		for (u32 i = 0; i < material.texturesCount; ++i) {
			const Texture& texture = textureManager->GetTexture(material.textures[i]);

			PipelineTexture* pipelineTexture = _textures.Find((h64) material.textures[i]);

			if (pipelineTexture) {
				outPipelineObject->textures[outPipelineObject->texturesCount++].handle = pipelineTexture->handle;
			}
			else {
				render::Texture renderTexture;
				render::InitResource(&renderTexture, _handleManager->NewHandle(), render::ResourceType::TEXTURE);
				renderTexture.minFilter = render::TextureFilterType::Linear;
				renderTexture.maxFilter = render::TextureFilterType::Linear;
				renderTexture.wrapS = render::TextureWrapType::Repeat;
				renderTexture.wrapT = render::TextureWrapType::Repeat;
				if (texture.format == TextureFormat::RGB) renderTexture.format = render::TextureFormat::RGB;
				else if (texture.format == TextureFormat::RGBA) renderTexture.format = render::TextureFormat::RGBA;
				else ASSERT(false);

				renderTexture.width = texture.width;
				renderTexture.height = texture.height;
				renderTexture.mipmaps = false;
				renderTexture.data = texture.data;

				outPipelineObject->textures[outPipelineObject->texturesCount++].handle = renderTexture.handle;

				_newResources.Push(render::TextureAllocate(stream, &renderTexture));


				PipelineTexture newPipelineTexture;
				newPipelineTexture.handle = renderTexture.handle;

				_textures.Add((h64) material.textures[i], newPipelineTexture);
			}
		}


		// @TODO material template
		// @TODO hashmap should be reworked to return pointers on add  // might not be needed
		// @TODO this will be reworked when resources arent allocated synchronously


		const ShaderProgram& program = shaderProgramManager->GetShaderProgram(material.shaderProgram);

		PipelineShader* pipelineShader = _shaders.Find((h64) material.shaderProgram);

		outPipelineObject->resourcesCount = 0;

		if (pipelineShader) {
			outPipelineObject->shaderHandle = pipelineShader->handle;
		}
		else {
			render::Shader shader;
			render::InitResource(&shader, _handleManager->NewHandle(), render::ResourceType::SHADER);
			shader.vertexStage.data = shaderStageManager->GetShaderStage(program.vertexStage).data;
			shader.fragmentStage.data = shaderStageManager->GetShaderStage(program.fragmentStage).data;

			outPipelineObject->shaderHandle = shader.handle;

			PipelineShader newPipelineShader;
			newPipelineShader.handle = shader.handle;

			_newResources.Push(ShaderAllocate(stream, &shader));

			_shaders.Add((h64) material.shaderProgram, newPipelineShader);
		}

		PipelineMesh* pipelineMesh = _meshes.Find((h64) renderObject->mesh);
		if (pipelineMesh) {
			outPipelineObject->vertices = pipelineMesh->verticesCount;
			outPipelineObject->indices = pipelineMesh->indicesCount;

			// @TODO Who should handle vertex decription renderer or pipeline... ?
			outPipelineObject->resources[outPipelineObject->resourcesCount++] = {pipelineMesh->vertexDescriptionHandle};
		}
		else {
			const Mesh& mesh = meshManager->GetMesh(renderObject->mesh);

			outPipelineObject->vertices = mesh.verticesCount;
			outPipelineObject->indices = mesh.indiciesCount;

			PipelineMesh newPipelineMesh;
			RegisterPipelineMesh(frame, stream, &mesh, &newPipelineMesh);
			outPipelineObject->resources[outPipelineObject->resourcesCount++] = { newPipelineMesh.vertexDescriptionHandle };
			_meshes.Add((h64) renderObject->mesh, newPipelineMesh);
		}
	}

	void RenderPipeline::RegisterPipelineMesh(const Frame* frame, render::RenderContextStream* stream, const Mesh* mesh, PipelineMesh* outPipelineMesh) {
		render::VertexDescription vertexDescription;
		render::VertexBuffer vertexBuffer;
		render::IndexBuffer indexBuffer;

		render::InitResource(&vertexBuffer, _handleManager->NewHandle(), render::ResourceType::VERTEX_BUFFER);
		render::InitResource(&indexBuffer, _handleManager->NewHandle(), render::ResourceType::INDEX_BUFFER);
		render::InitResource(&vertexDescription, _handleManager->NewHandle(), render::ResourceType::VERTEX_DESCRIPTION);

		indexBuffer.data = (const char*) mesh->indices;
		indexBuffer.dataSize = sizeof(u16) * mesh->indiciesCount;
		indexBuffer.bufferType = render::BufferType::STATIC;
		indexBuffer.indexType = render::IndexType::U16;

		vertexDescription.attributesCount = 0;
		vertexDescription.indexBufferHandle = indexBuffer.handle;
		vertexDescription.hasIndexBuffer = true;

		u32 bufferSize = 0;
		u32 attributeIndex = 0; // @TODO should be without strict order .. should be some attribute array generated, iterated and copied
		u32 attributeOffset = 0;

		// interleaved or not??? 

		if (mesh->positions) {
			ASSERT(mesh->positionsType == MeshBufferType::V2);
			bufferSize += sizeof(f32) * 2 * mesh->verticesCount;
			AddVertexAttribute(&vertexDescription, vertexBuffer.handle, attributeIndex, attributeOffset, 0, 0, render::VertexAttributeType::V2);
			attributeIndex++;
			attributeOffset += mesh->verticesCount * sizeof(f32) * 2;
		}

		if (mesh->uvs) {
			ASSERT(mesh->positionsType == MeshBufferType::V2);
			bufferSize += sizeof(f32) * 2 * mesh->verticesCount;
			AddVertexAttribute(&vertexDescription, vertexBuffer.handle, attributeIndex, attributeOffset, 0, 0, render::VertexAttributeType::V2);
			attributeIndex++;
			attributeOffset += mesh->verticesCount * sizeof(f32) * 2;
		}
		// @TODO should be align proof

		// @TODO we should make space for indices too so they can be changed

		char* attributesBuffer = (char*) TagAllocate(frame->tagAllocator, frame->renderPipelineTag, bufferSize, alignof(f32));
		vertexBuffer.data = attributesBuffer;
		
		if (mesh->positions) {
			core::Memcpy(attributesBuffer, mesh->positions, mesh->verticesCount * sizeof(f32) * 2);
			attributesBuffer += mesh->verticesCount * sizeof(f32) * 2;
		}

		if (mesh->uvs) {
			core::Memcpy(attributesBuffer, mesh->uvs, mesh->verticesCount * sizeof(f32) * 2);
			attributesBuffer += mesh->verticesCount * sizeof(f32) * 2;
		}

		vertexBuffer.dataSize = bufferSize;
		vertexBuffer.bufferType = render::BufferType::STATIC;

		outPipelineMesh->vertexDescriptionHandle = vertexDescription.handle;
		outPipelineMesh->vertexBufferHandle = vertexBuffer.handle;
		outPipelineMesh->indexBufferHandle = indexBuffer.handle;
		outPipelineMesh->verticesCount = mesh->verticesCount;
		outPipelineMesh->indicesCount = mesh->indiciesCount;

		render::ResourceContext resourceContext[3];
		resourceContext[0] = VertexBufferAllocate(stream, &vertexBuffer);
		resourceContext[1] = IndexBufferAllocate(stream, &indexBuffer);
		resourceContext[2] = VertexDescriptionAllocate(stream, &vertexDescription); // must be after VB IB

		_newResources.PushValues(resourceContext, 3);
	}
}

