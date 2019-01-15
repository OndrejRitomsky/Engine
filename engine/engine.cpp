#include "engine.h"

#include <core/common/types.h>
#include <core/common/placement.h>
#include <core/common/pointer.h>
#include <core/parallel/job_pool.h>
#include <core/allocator/allocate.h>


#include "frame/frame_module.h"

#include "memory/permanent_allocator.h"
#include "memory/memory_module.h"

#include "platform/platform_module.h"
#include "world/world_module.h"
#include "resources/resource_module.h"
#include "file_system/file_system_module.h"
#include "render/render_module.h"
#include "script/script_module.h"

#include "file_system/file_system_event.h"


/*  ONLY TOR TESTING REMOVE LATER*/

#include "data_to_load.h"

#include "world/entity.h"
#include "world/manager/scene.h"
#include "world/manager/transform2D_component_manager.h"

#include "render/manager/render_object_component_manager.h"

#include "script/manager/script_component_manager.h"

#include <core/math/v2.h>


namespace eng {

	// Maybe once modules will be really modular... 
	Engine EngineMake(core::IAllocator* mainAllocator) {
		using namespace core;

		PermanentAllocator* permanentAllocator;
		{
			u64 size = 1024 * 1024;
			PermanentAllocator perma;
			PermanentAllocatorInit(&perma, mainAllocator, size);

			permanentAllocator = PermanentNew(&perma, PermanentAllocator)();
			*permanentAllocator = perma;
		}


		Engine engine;
		engine.memoryModule = PermanentNew(permanentAllocator, MemoryModule)();
		engine.memoryModule->ConstructSubsytems(permanentAllocator);
		engine.memoryModule->Init(mainAllocator, permanentAllocator); // memory module stores permanentAllocator



		engine.platformModule = PermanentNew(permanentAllocator, PlatformModule)();
		engine.frameModule = PermanentNew(permanentAllocator, FrameModule)();
		engine.fileSystemModule = PermanentNew(permanentAllocator, FileSystemModule)();
		engine.resourceModule = PermanentNew(permanentAllocator, ResourceModule)();
		engine.worldModule = PermanentNew(permanentAllocator, WorldModule)();
		engine.scriptModule = PermanentNew(permanentAllocator, ScriptModule)();
		engine.renderModule = PermanentNew(permanentAllocator, RenderModule)();
		//engine.jobPool          = PermanentAllocate(permanentAllocator, JobPool)();



		engine.platformModule->ConstructSubsytems(permanentAllocator);
		engine.frameModule->ConstructSubsytems(permanentAllocator);
		engine.fileSystemModule->ConstructSubsytems(permanentAllocator);
		engine.resourceModule->ConstructSubsytems(permanentAllocator);
		engine.worldModule->ConstructSubsytems(permanentAllocator);
		engine.scriptModule->ConstructSubsytems(permanentAllocator);
		engine.renderModule->ConstructSubsytems(permanentAllocator);

		return engine;
	}

	void EngineInit(Engine* engine) {
		engine->platformModule->Init(engine, 1024, 768);
		engine->frameModule->Init(engine);
		engine->fileSystemModule->Init(engine);
		engine->resourceModule->Init(engine);
		engine->worldModule->Init(engine);
		engine->scriptModule->Init(engine);
		//engine->jobPool->Init();
		engine->renderModule->Init(engine, engine->platformModule->DeviceContext());
	}

	void EngineDestroy(Engine* engine) {
		//	engine->jobPool->Destroy();

		engine->renderModule->~RenderModule();
		engine->scriptModule->~ScriptModule();
		engine->worldModule->~WorldModule();
		engine->fileSystemModule->~FileSystemModule();
		engine->resourceModule->~ResourceModule();
		engine->frameModule->~FrameModule();
		engine->platformModule->~PlatformModule();

		PermanentAllocator* permanentAllocator = engine->memoryModule->PermaAllocator();
		engine->memoryModule->~MemoryModule();
		PermanentAllocatorDeinit(permanentAllocator);
	}



	// @TODO mesh positions : [] .. uvs : [] etc... whole arrays... then just check what is has ... and pipeline should create ONE VBO and interleave 
	// -> this should be controled by shader... but wont do it for now and only ONE MESH until needed

	// @TODO pipeline is not finished
	// @TODO alignment in buffers? solved in resources not in render commands ->  context->AddRenderJobPackage(&jobPackage); rework

	// @CHANGES: maybe we dont need pending resources... just create them
	// @CHANGES: maybe add mesh / render object should be private... since manager might allocate
	// @CHANGES who manages component resources (it should be scene... but its not thing trough.. and maybe only this component need resource? need to fake it for now)


	// @TODO 10. font renderer
	// @TODO 10.5 for omgui we need a way to add custom things to renderer
	// @TODO 11. omgui (check network hookups)

	// @TODO x. entities could move with scripts
	// @TODO x. renderer shouldnt MAP handles... it should return GL handles back to engine renderer
	// @TODO x. editor

	// @TODO xx. remove container base...
	// @TODO xx. renderer shouldnt use virtuals.. (nothing should)
	// @TODO xx. renderer could be hidden ... (need common init function) (struct with init date?)


	void EnginePrepareData(Engine* engine) {

		eng::LoadShaders(engine);
		eng::LoadPrograms(engine);
		eng::LoadTextures(engine);
		eng::LoadMaterialTemplates(engine);
		eng::LoadMaterials(engine);
		eng::LoadMeshes(engine);
		eng::LoadRenderObjects(engine);

		auto worldModule = engine->worldModule;
		auto renderModule = engine->renderModule;


		eng::Scene* scene = worldModule->GetScene();
		eng::Transform2DComponentManager* transforms = worldModule->GetTransform2DComponentManager();
		eng::RenderObjectComponentManager* renderObjects = renderModule->GetRenderObjectComponentManager();
		eng::ScriptComponentManager* scriptManager = engine->scriptModule->GetScriptComponentManager();

		{
			eng::Entity entity = scene->AddEntity();
			auto renderComponent = renderObjects->Create(entity);

			const char* path = "assets/render_object/box_render_object.json";
			h64 hash = core::Hash(path, (u32)core::Strlen(path));
			renderObjects->SetResourceHash(renderComponent, hash);

			auto transformComponent = transforms->Create(entity);
			transforms->SetAngle(transformComponent, 0);
			transforms->SetPosition(transformComponent, V2(0, 0));
			transforms->SetAngle(transformComponent, 0);
			transforms->SetScale(transformComponent, V2(100, 100));

			scriptManager->Create(entity, "ScriptTest");
		}

		{
			eng::Entity entity = scene->AddEntity();
			auto renderComponent = renderObjects->Create(entity);

			const char* path = "assets/render_object/textured_box_render_object.json";
			h64 hash = core::Hash(path, (u32)core::Strlen(path));
			renderObjects->SetResourceHash(renderComponent, hash);

			auto transformComponent = transforms->Create(entity);
			transforms->SetAngle(transformComponent, 0);
			transforms->SetPosition(transformComponent, V2(200, 0));
			transforms->SetAngle(transformComponent, 0);
			transforms->SetScale(transformComponent, V2(100, 100));

			scriptManager->Create(entity, "ScriptTest");
		}
	}

	bool EngineRun(Engine* engine) {
		eng::FileSystemModule* fsm = engine->fileSystemModule;
		eng::ResourceModule* rsm = engine->resourceModule;
		eng::PlatformModule* platformModule = engine->platformModule;
		eng::WorldModule* worldModule = engine->worldModule;
		eng::RenderModule* renderModule = engine->renderModule;
		eng::ScriptModule* scriptModule = engine->scriptModule;

		EnginePrepareData(engine);

		int f = 0;
		while (true) {

			/*Log("test frame %d \n", f);
			f++;*/

			engine->frameModule->Update();

			const Frame* frame = engine->frameModule->CurrentFrame();

			fsm->Update(frame);
			rsm->Update(frame);
			worldModule->Update(frame);
			platformModule->Update(frame);
			scriptModule->Update(frame);

			// @TODO Rework platform event system if there is too many events to pool for! (currently there isnt many)
			if (platformModule->CloseRequested()) {
				break;
			}

			{
				i32 w, h;
				if (platformModule->WindowSize(&w, &h)) {
					renderModule->SetViewPort(w, h);
					//RendererResize(&engine->renderer, w, h);
				}
			}

			{
				// @TODO init function for events
				eng::ResourceLoadEvents el;
				rsm->QueryEventsByType(eng::ResourceEventType::FS_LOAD, &el);
				if (el.count > 0) {
					FileSystemLoadEvents evt;
					evt.count = el.count;
					evt.hashes = el.hashes;
					evt.extraData = el.typeIDs;
					fsm->OnEventsByType(frame, &evt, FileSystemEventType::LOAD);
				}
			}

			{
				eng::FileSystemLoadedEvents loadedEvents;
				fsm->QueryEventsByType(eng::FileSystemEventType::LOADED, &loadedEvents);
				if (loadedEvents.count) {
					ResourceDataLoadedEvents events;
					events.count = loadedEvents.count;
					events.bufferSizes = loadedEvents.bufferSizes;
					events.hashes = loadedEvents.nameHashes;
					events.typeIDs = loadedEvents.extraData;
					events.buffers = loadedEvents.buffers;
					rsm->OnEventsByType(frame, &events, ResourceEventType::DATA_LOADED);
				}
			}

			{
				eng::ResourceLoadedEvents loadedEvents;
				rsm->QueryEventsByType(eng::ResourceEventType::LOADED, &loadedEvents);
				if (loadedEvents.count > 0) { // @TODO it would be great to know if its actualy render object ... should add type id
					RenderResourcesLoadedEvents evt;
					evt.count = loadedEvents.count;
					evt.hashes = loadedEvents.hashes;
					renderModule->OnEventsByType(frame, &evt, RenderEventType::RESOURCES_LOADED);
				}
			}

			renderModule->Update(frame);

			rsm->ClearPendingEvents();
			fsm->ClearPendingEvents();
			worldModule->ClearPendingEvents();

			platformModule->SwapBackBuffers(); // move to renderer
		}

		return true;
	}
}
