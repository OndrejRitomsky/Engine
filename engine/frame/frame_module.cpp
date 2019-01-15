#include "frame_module.h"

#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/platform/timer.h>

#include "frame.h"

#include "../Engine.h"

#include "../common/log.h"

#include "../memory/permanent_allocator.h"
#include "../memory/memory_module.h"





namespace eng {
	static const u64 TAGS_MEMORY_COUNT = 8;
	static const u64 ALIVE_TAGS_COUNT = 4; // 4: PREVIOUS2 PREVIOUS1 CURRENT FUTURE (FUTURE2 - if previous2 was just destroyed)
	static const u64 HISTORY_TAGS_COUNT = 2;  // should be as much as pending tasks need (mainly renderer, many frame tasks shouldnt allocate with frame tag)
	static const u64 FUTURE_TAGS_COUNT = ALIVE_TAGS_COUNT - HISTORY_TAGS_COUNT;

	static_assert(TAGS_MEMORY_COUNT > ALIVE_TAGS_COUNT, "TAGS SIZE MISSMATCH");
	static_assert(ALIVE_TAGS_COUNT > 0, "TAGS SIZE MISSMATCH");
	static_assert(HISTORY_TAGS_COUNT > 0, "TAGS SIZE MISSMATCH");
	static_assert(FUTURE_TAGS_COUNT > 0, "TAGS SIZE MISSMATCH");
	static_assert(FUTURE_TAGS_COUNT < ALIVE_TAGS_COUNT, "TAGS SIZE MISSMATCH");

	static void FrameInit(Frame* frame, MemoryModule* mm, u64 index) {
		frame->frameIndex = index;
		frame->tagAllocator = mm->TagAllocator();


		core::MemTag tags[3];
		mm->MakeMemTag(3, tags);

		frame->gameTag = tags[0];
		frame->renderPipelineTag = tags[1];
		frame->rendererTag = tags[2];
	}

	static void FrameRelease(Frame* frame, MemoryModule* mm) {
		core::MemTag tags[3];
		tags[0] = frame->gameTag;
		tags[1] = frame->renderPipelineTag;
		tags[2] = frame->rendererTag;

		auto* all = mm->TagAllocator();
		for (u32 i = 0; i < 3; ++i)
			TagDeallocate(all, tags[i]); // Release the memory

		mm->ReleaseMemTag(tags, 3);

		frame->frameIndex = 0;
		frame->gameTag = 0;
		frame->renderPipelineTag = 0;
		frame->rendererTag = 0;
	}

	FrameModule::FrameModule() :
		_state(ModuleState::CREATED),
		_memoryModule(nullptr),
		_frames(nullptr),
		_framesTag(0),
		_frameCount(0),
		_currentFrame(0),
		_frameGlobalNumber(0) {
	}

	FrameModule::~FrameModule() {
		if (_state == ModuleState::OK) {
			for (u32 i = 0; i < ALIVE_TAGS_COUNT; ++i) {
				u64 j = (_currentFrame - HISTORY_TAGS_COUNT + i + TAGS_MEMORY_COUNT) % TAGS_MEMORY_COUNT;
				FrameRelease(_frames + j, _memoryModule);
			}

			TagDeallocate(_memoryModule->TagAllocator(), _framesTag);
			_memoryModule->ReleaseMemTag(&_framesTag, 1);
		}
	}

	ModuleState FrameModule::State() {
		return _state;
	}

	void FrameModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;
	}

	void FrameModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		_memoryModule = engine->memoryModule;
		_memoryModule->MakeMemTag(1, &_framesTag);


		_frameCount = TAGS_MEMORY_COUNT;
		u64 size = _frameCount * sizeof(Frame);
		u64 outSize;
		void* data = TagAllocate(_memoryModule->TagAllocator(), _framesTag, size, 128, &outSize);
		_frames = static_cast<Frame*>(data);

		// ALIVE_TAGS_COUNT tags are always prepared (with memory tags) previous current next
		for (u32 i = 0; i < ALIVE_TAGS_COUNT; ++i) {
			FrameInit(_frames + i, _memoryModule, _frameGlobalNumber++);
		}
		// the 0`th has to be created for deleting previous
		_currentFrame = HISTORY_TAGS_COUNT;
	}

	void FrameModule::Update() {
		//Log("frame index %llu\n", _currentFrame);

		u64 old;
		if (_currentFrame < 2) {
			old = _currentFrame + TAGS_MEMORY_COUNT - HISTORY_TAGS_COUNT;
		}
		else {
			old = _currentFrame - HISTORY_TAGS_COUNT;
		}

		FrameRelease(_frames + old, _memoryModule);

		u64 nextToCreate = (_currentFrame + FUTURE_TAGS_COUNT) % TAGS_MEMORY_COUNT;
		FrameInit(_frames + nextToCreate, _memoryModule, _frameGlobalNumber++);
		_frames[nextToCreate].tagAllocator = _memoryModule->TagAllocator();

		_currentFrame = (_currentFrame + 1) % TAGS_MEMORY_COUNT;

		core::Sleep(20); // @TODO timer
	}

	const Frame* FrameModule::CurrentFrame() {
		return _frames + _currentFrame;
	}
}

