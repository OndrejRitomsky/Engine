

#include "../core/allocator/allocators.h"
#include "../core/allocator/allocate.h"


#include "engine/engine.h"



#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	core::HeapAllocator heapAll;
	core::IAllocator* allocator = HeapAllocatorInit(&heapAll);

	eng::Engine engine = eng::EngineMake(allocator);
	eng::EngineInit(&engine);
	eng::EngineRun(&engine);
	eng::EngineDestroy(&engine);

	AllocatorDeinit(allocator);

	return 0;
}
