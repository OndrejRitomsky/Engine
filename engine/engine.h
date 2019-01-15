#pragma once

#include "common/export.h"


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
	class FileSystemModule;
	class PlatformModule;
	class WorldModule;
	class ScriptModule;

	struct Engine {
		MemoryModule* memoryModule;
		ResourceModule* resourceModule;
		RenderModule* renderModule;
		FrameModule* frameModule;
		PlatformModule* platformModule;
		WorldModule* worldModule;
		ScriptModule* scriptModule;

		FileSystemModule* fileSystemModule;

		core::JobPool* jobPool;

		//Renderer renderer;
	};

	ENG_EXPORT Engine EngineMake(core::IAllocator* mainAllocator);

	ENG_EXPORT void EngineInit(Engine* engine);

	ENG_EXPORT bool EngineRun(Engine* engine);
	ENG_EXPORT void EngineDestroy(Engine* engine);
}