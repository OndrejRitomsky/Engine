#include "Engine.h"

#include <Core/Common/Types.h>
#include <Core/Common/Placement.h>
#include <Core/Common/Pointer.h>
#include <Core/Parallel/JobPool.h>
#include <Core/Allocator/IAllocator.h>


#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Memory/CMemoryModule.h"

#include "Engine/Resources/CResourceModule.h"

#include "Engine/FileSystem/CFileSystemModule.h"

#include "Engine/RenderPipeline/CRenderModule.h"

#include "Engine/Frame/CFrameModule.h"

/*  ONLY TOR TESTING REMOVE LATER*/

#include "DataToLoad.h"

#include "Engine/FileSystem/FileSystemEvent.h"

#include <Core/Common/Types.h>
#include <Core/Algorithm/Hash.h>

/*  ONLY TOR TESTING REMOVE LATER*/


namespace eng {


	//---------------------------------------------------------------------------
	void EngineDestroy(Engine* engine) {
	//	engine->jobPool->Destroy();

		engine->renderModule->~CRenderModule();
		engine->fileSystemModule->~CFileSystemModule();
		engine->jobPool->~JobPool();
		engine->resourceModule->~CResourceModule();
		engine->frameModule->~CFrameModule();

		//-------------------------------------------------------------------------
		char* data = static_cast<char*>(engine->_constructor->GetBuffer()) - sizeof(CStaticConstructor);
		core::IAllocator* mainAllocator = engine->memoryModule->Allocator();

		engine->memoryModule->~CMemoryModule();
		engine->_constructor->~CStaticConstructor();

		mainAllocator->Deallocate(data);
	}

	// Maybe once modules will be really modular... 
	//---------------------------------------------------------------------------
	Engine EngineMake(core::IAllocator* mainAllocator) {
		using namespace core;

		u64 size = 2 * 1024 * 1024;
		char* data = static_cast<char*>(mainAllocator->Allocate(size, alignof(CStaticConstructor)));

		Engine engine;
		engine._constructor = Placement(data) CStaticConstructor();
		engine._constructor->Init(data + sizeof(CStaticConstructor), size - sizeof(CStaticConstructor));

		CStaticConstructor* constructor = engine._constructor;

		engine.memoryModule = constructor->Construct<CMemoryModule>();
		engine.memoryModule->ConstructSubsytems(constructor);
		engine.memoryModule->Init(mainAllocator);

		//-------------------------------------------------------------------------

		engine.frameModule = constructor->Construct<CFrameModule>();
		engine.fileSystemModule = constructor->Construct<CFileSystemModule>();
		engine.resourceModule = constructor->Construct<CResourceModule>();
		engine.renderModule = constructor->Construct<CRenderModule>();
		engine.jobPool = constructor->Construct<JobPool>();


		engine.frameModule->ConstructSubsytems(constructor);
		engine.fileSystemModule->ConstructSubsytems(constructor);
		engine.resourceModule->ConstructSubsytems(constructor);
		engine.renderModule->ConstructSubsytems(constructor);

		return engine;
	}

	//---------------------------------------------------------------------------
	void EngineInitModules(Engine* engine) {

		engine->frameModule->Init(engine);
		engine->fileSystemModule->Init(engine);
		engine->resourceModule->Init(engine);
		//engine->jobPool->Init();
		engine->renderModule->Init(engine);
	}

	//---------------------------------------------------------------------------
	void EngineRun(Engine* engine) {
		eng::CFileSystemModule* fsm = engine->fileSystemModule;
		eng::CResourceModule* rsm = engine->resourceModule;

		u32 toLoad = 0;

		toLoad += eng::LoadShaders(engine);
		toLoad += eng::LoadPrograms(engine);
		//toLoad += eng::LoadTextures(engine);
		//toLoad += eng::LoadMaterialTemplates(engine);
	//	toLoad += eng::LoadMaterials(engine);
		//toLoad += eng::LoadMeshes(engine);

		u32 got = 0;
		while (got < toLoad) {
			const Frame* frame = engine->frameModule->CurrentFrame();

			// @TODO init function for events
			eng::ResourceLoadEvents el;
			rsm->QueryEventsByType(eng::ResourceEventType::LOAD, &el);
			if (el.count > 0) {
				FileSystemLoadEvents evt;
				evt.count = el.count;
				evt.hashes = el.hashes;
				evt.extraData = el.typeIDs;
				fsm->OnEventsByType(&evt, FileSystemEventType::LOAD);
			}

			eng::FileSystemLoadedEvents fsl;
			fsm->QueryEventsByType(eng::FileSystemEventType::LOADED, &fsl);

			if (fsl.count) {			
				ResourceDataLoadedEvents le;
				le.count = fsl.count;
				le.bufferSizes = fsl.bufferSizes;
				le.hashes = fsl.nameHashes;
				le.typeIDs = fsl.extraData;
				le.buffers = fsl.buffers;
				rsm->OnEventsByType(&le, ResourceEventType::DATA_LOADED);
			}

			rsm->ClearPendingEvents();
			fsm->ClearPendingEvents();


			// @TODO MOVE UP
			fsm->Update(frame);
			rsm->Update(frame);
		}

	}	
}