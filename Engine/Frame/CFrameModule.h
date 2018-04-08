#pragma once

#include <Core/Common/Types.h>
#include <Core/TagAllocator/MemTag.h>

#include "Engine/Modules/ModuleState.h"


namespace eng {
	struct Engine;
	struct Frame;

	class CStaticConstructor;
	class CMemoryModule;

	class CFrameModule {
	public:
		CFrameModule();
		~CFrameModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
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

		u32 _currentFrame;
		u32 _frameCount;

		Frame* _frames;

		CMemoryModule* _memoryModule;

		core::MemTag _framesTag;
	};

}