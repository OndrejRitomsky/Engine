#include "engine.h"

#include <core/common/types.h>
#include <core/common/placement.h>
#include <core/common/pointer.h>
#include <core/parallel/job_pool.h>
#include <core/allocator/allocate.h>


#include "frame/frame.h"
#include "frame/frame_module.h"

#include "memory/permanent_allocator.h"
#include "memory/memory_module.h"

#include "platform/platform_module.h"
#include "world/world_module.h"
#include "resources/resource_module.h"
#include "FileSystem/CFileSystemModule.h"
#include "render/render_module.h"




/*  ONLY TOR TESTING REMOVE LATER*/

#include "DataToLoad.h"

#include "FileSystem/FileSystemEvent.h"

#include <core/common/types.h>
#include <core/algorithm/hash.h>
#include <core/platform/timer.h>

/*  ONLY TOR TESTING REMOVE LATER*/

#include "common/log.h"

#include "world/entity.h"
#include "world/manager/scene.h"
#include "world/manager/CTransform2DComponentManager.h"
#include "world/manager/render_object_component_manager.h"

#include <core/math/v2.h>


namespace eng {


	void EngineDestroy(Engine* engine) {
	//	engine->jobPool->Destroy();

		engine->renderModule->~RenderModule();
		engine->worldModule->~WorldModule();
		engine->fileSystemModule->~CFileSystemModule();
		engine->resourceModule->~ResourceModule();
		engine->frameModule->~FrameModule();
		engine->platformModule->~PlatformModule();

		PermanentAllocator* permanentAllocator = engine->memoryModule->PermaAllocator();
		engine->memoryModule->~MemoryModule();
		PermanentAllocatorDeinit(permanentAllocator);
	}

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
	
		engine.platformModule   = PermanentNew(permanentAllocator, PlatformModule)();
		engine.frameModule      = PermanentNew(permanentAllocator, FrameModule)();
		engine.fileSystemModule = PermanentNew(permanentAllocator, CFileSystemModule)();
		engine.resourceModule   = PermanentNew(permanentAllocator, ResourceModule)();
		engine.worldModule      = PermanentNew(permanentAllocator, WorldModule)();
		engine.renderModule     = PermanentNew(permanentAllocator, RenderModule)();
		//engine.jobPool          = PermanentAllocate(permanentAllocator, JobPool)();

		engine.platformModule->ConstructSubsytems(permanentAllocator);
		engine.frameModule->ConstructSubsytems(permanentAllocator);
		engine.fileSystemModule->ConstructSubsytems(permanentAllocator);
		engine.resourceModule->ConstructSubsytems(permanentAllocator);
		engine.worldModule->ConstructSubsytems(permanentAllocator);
		engine.renderModule->ConstructSubsytems(permanentAllocator);

		return engine;
	}

	void EngineInitModules(Engine* engine) {

		engine->platformModule->Init(engine, 1024, 768);
		engine->frameModule->Init(engine);
		engine->fileSystemModule->Init(engine);
		engine->resourceModule->Init(engine);
		engine->worldModule->Init(engine);
		//engine->jobPool->Init();
		engine->renderModule->Init(engine, engine->platformModule->DeviceContext());

		//RendererInit(&engine->renderer, engine->platformModule->DeviceContext(), engine->memoryModule->Allocator());
	}

	// @CHANGES: maybe we dont need pending resources... just create them
	// @CHANGES: maybe add mesh / render object should be private... since manager might allocate

	// @TODO 0. proper logging would be good>... (take from network)
	
	// @TODO 3. UPDATE only material fixes bcs of shared shader program fix after usage, mesh + material managers -> again json ... should look same as shader program

	
	// @TODO Need to rework render object componetns.. we need resource not hash... so it can be counted
	//       but who will manage this? (it should be scene... but its not thing trough.. and maybe only this component need resource? need to fake it for now)

	// @TODO 6. render module needs see new entities with new materials and request from resource manager loads (it would be nice to count how many frames this takes... need Outputdebug for logging)
	// @TODO 7. when materials come send to renderer
	// @TODO 8. render entities which can be rendered
	// @TODO 9. textured entities
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
		//eng::LoadTextures(engine);
		eng::LoadMaterialTemplates(engine);
		eng::LoadMaterials(engine);
		eng::LoadMeshes(engine);
		eng::LoadRenderObjects(engine);
		
		auto wm = engine->worldModule;


		eng::Scene* scene                    = wm->GetScene();
		eng::CTransform2DComponentManager* transforms    = wm->GetTransform2DComponentManager();
		eng::RenderObjectComponentManager* renderObjects = wm->GetRenderObjectComponentManager();

		{
			eng::Entity entity;
			scene->AddEntity(&entity);
			auto renderComponent = renderObjects->Create(&entity);

			const char* path = "assets/render_object/box_render_object.json";	
			h64 hash = core::Hash(path, (u32) core::Strlen(path));
			renderObjects->SetResourceHash(renderComponent, hash);

			auto transformComponent = transforms->Create(&entity);
			transforms->SetAngle(transformComponent, 0);
			transforms->SetPosition(transformComponent, V2(100,100));
			transforms->SetScale(transformComponent, V2(1,1));
		}
	}

	void EngineRun(Engine* engine) {
		eng::CFileSystemModule* fsm = engine->fileSystemModule;
		eng::ResourceModule* rsm = engine->resourceModule;
		eng::PlatformModule* platformModule = engine->platformModule;
		eng::WorldModule* worldModule = engine->worldModule;
		eng::RenderModule* renderModule = engine->renderModule;
		
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

			// @TODO Rework platform event system if there is too many events to pool for! (currently there isnt many)
			if (platformModule->CloseRequested()) {
				break;
			}
			{
				i32 w, h;
				if (platformModule->WindowSize(&w, &h)) {
					//RendererResize(&engine->renderer, w, h);
				}
			}
			{
				// @TODO init function for events
				{
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
					eng::FileSystemLoadedEvents fsl;
					fsm->QueryEventsByType(eng::FileSystemEventType::LOADED, &fsl);

					if (fsl.count) {
						ResourceDataLoadedEvents le;
						le.count = fsl.count;
						le.bufferSizes = fsl.bufferSizes;
						le.hashes = fsl.nameHashes;
						le.typeIDs = fsl.extraData;
						le.buffers = fsl.buffers;
						rsm->OnEventsByType(frame, &le, ResourceEventType::DATA_LOADED);
					}
				}
				{
					eng::WorldRenderEvents fsl;
					worldModule->QueryEventsByType(eng::WorldEventType::RENDER, &fsl);

					if (fsl.count) {
						//RenderRenderEvents re;
						//ResourceDataLoadedEvents le;
						//le.count = fsl.count;
						//le.bufferSizes = fsl.bufferSizes;

						//renderModule->OnEventsByType(frame, &le, ResourceEventType::DATA_LOADED);
					}
				}


				rsm->ClearPendingEvents();
				fsm->ClearPendingEvents();
				worldModule->ClearPendingEvents();
			}




			/*RendererClear();
			RendererDrawRectangle(&engine->renderer, 40, 40, 50, 50, 1, 0, 0);*/

			
			platformModule->SwapBackBuffers(); // move to renderer
			core::Sleep(100); // move to frame module?
		}

	}	
}