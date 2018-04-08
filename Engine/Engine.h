#pragma once

#include "Engine/Export.h"

namespace core {
	class IAllocator;
	class JobPool;
}

namespace eng {
	class CResourceModule;
	class CMemoryModule;
	class CRenderModule;
	class CFrameModule;
	class CStaticConstructor;
	class CFileSystemModule;


	struct Engine {
		CMemoryModule* memoryModule;
		CResourceModule* resourceModule;
		CRenderModule* renderModule;
		CFrameModule* frameModule;

		CFileSystemModule* fileSystemModule;

		core::JobPool* jobPool;
		CStaticConstructor* _constructor;
	};

	// @TODO doesnt really need 4 functions
	ENG_EXPORT Engine EngineMake(core::IAllocator* mainAllocator);
	ENG_EXPORT void EngineInitModules(Engine* engine);
	ENG_EXPORT void EngineRun(Engine* engine);
	ENG_EXPORT void EngineDestroy(Engine* engine);
}