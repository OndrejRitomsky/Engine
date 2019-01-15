#pragma once

#include <core/common/types.h>

#include "../module.h"


namespace eng {
	struct Engine;
	struct Frame;
	struct PermanentAllocator;

	class MemoryModule;

	class FrameModule {
	public:
		FrameModule();
		~FrameModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(Engine* engine);

		// Creates new frame with new tags
		// Alive frames: -2 -1 0 1
		// -2 is released, 0 is current engine frame
		// 1 is the new current (2 is prepared)
		void Update();

		// Current is depentant on the system, therefore engine should only call this
		const Frame* CurrentFrame();

	private:
		ModuleState _state;

		u64 _frameGlobalNumber;
		u64 _currentFrame;
		u64 _frameCount;

		Frame* _frames;

		MemoryModule* _memoryModule;

		core::MemTag _framesTag;
	};

}