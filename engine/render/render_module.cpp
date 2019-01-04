#include "render_module.h"

#include <core/common/debug.h>
#include <core/algorithm/hash.h>

#include <renderer/api/irender_device.h>
#include <renderer/gl/gl_render_device.h>

#include "render_internal_event.h"

#include "../engine.h"

#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"

#include "manager/render_resource_manager.h"
#include "manager/render_pipeline.h"



//@TEMP
#include <renderer/api/render_resources.h>
#include <renderer/api/render_context.h>
#include <renderer/api/render_resource_context.h>
#include <renderer/api/render_context_stream.h>


#include <renderer/api/command/job_package.h>
#include <renderer/api/command/uniform.h>
#include <renderer/api/command/command_parameters.h>

namespace eng {

	enum class RendererType : u8 {
		OpenGL,
	};

	RenderModule::RenderModule() :
		_state(ModuleState::CREATED),
		_renderDevice(nullptr) {
	}

	RenderModule::~RenderModule() {
		_renderDevice->~IRenderDevice();
	}

	ModuleState RenderModule::State() {
		return _state;
	}

	void RenderModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_renderDevice = (render::IRenderDevice*)(PermanentNew(permanentAllocator, render::GLRenderDevice)());
		_rendererType = static_cast<u8>(RendererType::OpenGL);

		_renderPipeline = PermanentNew(permanentAllocator, RenderPipeline)();
		_resourceManager = PermanentNew(permanentAllocator, RenderResourceManager)();
	}

	void RenderModule::Init(Engine* engine, void* deviceContext) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();

		static_cast<render::GLRenderDevice*>(_renderDevice)->Init(all, all, core::Hash, deviceContext); // We need type identifier

		_renderPipeline->Init(engine);
		_resourceManager->Init(engine);
	}

	render::Shader LoadShader(u32 handle) {
		render::Shader s1;
		render::InitResource(&s1, handle, render::ResourceType::SHADER);
		{
			char vName[] = "Shaders/vertex.glsl";
			char fName[] = "Shaders/fragment.glsl";

		/*	auto* a = shaderProgramManagerNew->GetShaderStage(core::Hash(vName, (u32) strlen(vName)));
			auto* b = shaderProgramManagerNew->GetShaderStage(core::Hash(fName, (u32) strlen(fName)));
			s1.vertexStage.dataSize = a->dataSize;
			s1.fragmentStage.data = b->data;*/
		}

		return s1;
	}

	void OnNewShader() {
		/*static bool called = false;
		ASSERT(!called);
		called = true;

		render::IndexBuffer i1;
		render::InitResource(&i1, 0, render::ResourceType::INDEX_BUFFER);
		i1.data = (char*) indices;
		i1.dataSize = sizeof(indices);
		i1.bufferType = render::BufferType::STATIC;
		i1.indexType = render::IndexType::U32;

		render::VertexBuffer v1;
		render::InitResource(&v1, handleManager.NewHandle(), render::ResourceType::VERTEX_BUFFER);
		v1.data = (char*) vertices;
		v1.dataSize = sizeof(vertices);
		v1.bufferType = render::BufferType::STATIC;

		render::VertexDescription vi1;
		render::InitResource(&vi1, handleManager.NewHandle(), render::ResourceType::VERTEX_DESCRIPTION);
		vi1.attributesCount = 0;
		vi1.indexBufferHandle = i1.handle;
		AddVertexAttribute(&vi1, v1.handle, 0, 0, 16, 0, render::VertexAttributeType::V2);
		AddVertexAttribute(&vi1, v1.handle, 1, 8, 16, 0, render::VertexAttributeType::V2);


		core::SafeTaggedBlockAllocator taggedAllocator;
		taggedAllocator.Init(_allocator, 1024 * 1024, 8);
		core::SingleTagProxyAllocator renderAllocator(&taggedAllocator, 14);

		render::RenderContextStream stream;
		stream.Init(&renderAllocator);

		render::RenderResourceContext resContext[4];
		resContext[0].Allocate(&stream, &s1);
		resContext[1].Allocate(&stream, &v1);
		resContext[2].Allocate(&stream, &i1);
		resContext[3].Allocate(&stream, &vi1);*/

	}

	void RenderModule::Update(const Frame* frame) {
		// This transforms RenderRenderEvents into RenderEntities, might be directly but its module input
		//_resourceManager->Update(frame);
		//
		//RenderEntityEvents entitiesEvent;
		//_resourceManager->GetRenderableEntities(&entitiesEvent);
		//
		//_renderPipeline->Update(frame, &entitiesEvent);



		/*render::BatchDescription batch;
		batch.instances = 1;
		batch.vertices = 0; // doesnt matter
		batch.indices = sizeof(indices) / sizeof(float);
		batch.type = render::BatchType::TRIANGLES;
		batch.indexType = render::IndexType::U32; // -> actualy device could hold this

		render::JobPackage jobPackage;
		jobPackage.batch = batch;
		jobPackage.shader = s1;
		jobPackage.size = sizeof(render::JobPackage) + 1 * sizeof(render::Resource) + 1 * sizeof(render::Sampler2DUniform);
		jobPackage.resourcesOffset = sizeof(render::JobPackage);
		jobPackage.resourcesCount = 1;
		jobPackage.uniformsOffset = jobPackage.resourcesOffset + 1 * sizeof(render::Resource);
		jobPackage.uniformsCount = 1;
		jobPackage.sortKey = 1;

		// @TODO figure out render package, how will the data for rendering be stored, before we send it to render context

		render::RenderContext renderContexts[2];
		renderContexts[0].Init(&stream);
		renderContexts[0].BeginStateCommands(0);
		renderContexts[0].AddClearCommand(render::ClearFlags::COLOR);
		renderContexts[0].EndStateCommands();

		renderContexts[1].Init(&stream);
		render::JobPackage* rjp = renderContexts[1].AddRenderJobPackage(&jobPackage);
		render::Resource* resource = (render::Resource*) core::PointerAdd(rjp, rjp->resourcesOffset);
		resource[0] = *((render::Resource*) &vi1);*/
	}

	void RenderModule::OnEventsByType(const Frame* frame, const void* events, RenderEventType eventsType) {
		switch (eventsType) {
		case RenderEventType::RENDER:
		{
			const RenderRenderEvents* evt = (const RenderRenderEvents*) events;
			_resourceManager->OnRenderable(evt);
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

	void RenderModule::QueryEventsByType(RenderEventType type, void* outEvents) {
		switch (type) {
		case RenderEventType::RENDER:
		{

			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}

	void RenderModule::ClearPendingEvents() {

	}









	render::IRenderDevice* RenderModule::RenderDevice() {
		return _renderDevice;
	}
}

