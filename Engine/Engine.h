#pragma once

#include "Engine/export.h"


//#include "Renderer.h"

namespace core {
	struct IAllocator;
	class JobPool;
}

namespace eng {
	class ResourceModule;
	class MemoryModule;
	class RenderModule;
	class FrameModule;
	class CFileSystemModule;
	class PlatformModule;
	class WorldModule;

	struct Engine {
		MemoryModule* memoryModule;
		ResourceModule* resourceModule;
		RenderModule* renderModule;
		FrameModule* frameModule;
		PlatformModule* platformModule;
		WorldModule* worldModule;

		CFileSystemModule* fileSystemModule;

		core::JobPool* jobPool;

		//Renderer renderer;
	};

	// @TODO doesnt really need 4 functions
	ENG_EXPORT Engine EngineMake(core::IAllocator* mainAllocator);
	ENG_EXPORT void EngineInitModules(Engine* engine);
	ENG_EXPORT void EngineRun(Engine* engine);
	ENG_EXPORT void EngineDestroy(Engine* engine);
}