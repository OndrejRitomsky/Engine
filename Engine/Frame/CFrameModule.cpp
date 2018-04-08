#include "CFrameModule.h"

#include <Core/Common/Assert.h>
#include <Core/TagAllocator/ITagAllocator.inl>

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Memory/CMemoryModule.h"

#include "Engine/Frame/Frame.h"


namespace eng {
	static const i32 TAGS_MEMORY_COUNT = 8;
	static const i32 ALIVE_TAGS_COUNT = 4;


	//---------------------------------------------------------------------------
	void CreateFrameTags(CMemoryModule* mm, Frame* frame) {
		core::MemTag tags[3];
		mm->MakeMemTag(3, tags);

		frame->gameTag = tags[0];
		frame->renderPipelineTag = tags[1];
		frame->rendererTag = tags[2];
	}


	//---------------------------------------------------------------------------
	void ReleaseFrameTags(CMemoryModule* mm, Frame* frame) {
		core::MemTag tags[3];
		tags[0] = frame->gameTag;
		tags[1] = frame->renderPipelineTag;
		tags[2] = frame->rendererTag;

		auto* all = mm->TagAllocator();
		for (u32 i = 0; i < 3; ++i)
			all->DeallocateAllWithTag(tags[i]); // Release the memory

		mm->ReleaseMemTag(tags, 3);
	}

	//---------------------------------------------------------------------------
	CFrameModule::CFrameModule() :
		_state(ModuleState::CREATED),
		_memoryModule(nullptr),
		_frames(nullptr),
		_framesTag(0),
		_frameCount(0),
		_currentFrame(0) {
	}

	//---------------------------------------------------------------------------
	CFrameModule::~CFrameModule() {
		if (_state == ModuleState::OK) {
			for (u32 i = 0; i < ALIVE_TAGS_COUNT; ++i) {
				i32 j = (_currentFrame - 2 + i + TAGS_MEMORY_COUNT) % TAGS_MEMORY_COUNT;
				ReleaseFrameTags(_memoryModule, _frames + j);
			}


			_memoryModule->TagAllocator()->DeallocateAllWithTag(_framesTag);
			_memoryModule->ReleaseMemTag(&_framesTag, 1);
		}
	}

	//---------------------------------------------------------------------------
	ModuleState CFrameModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CFrameModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;
	}

	//---------------------------------------------------------------------------
	void CFrameModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		_memoryModule = engine->memoryModule;
		_memoryModule->MakeMemTag(1, &_framesTag);


		_frameCount = TAGS_MEMORY_COUNT;
		u64 size = _frameCount * sizeof(Frame);
		u64 outSize;
		void* data = _memoryModule->TagAllocator()->AllocateWithTag(_framesTag, size, 64, &outSize);
		_frames = static_cast<Frame*>(data);

		// ALIVE_TAGS_COUNT tags are always prepared (with memory tags) previous current next
		for (u32 i = 0; i < ALIVE_TAGS_COUNT; ++i) {
			CreateFrameTags(_memoryModule, _frames + i);
		}
		// the 0`th has to be created for deleting previous
		_currentFrame = 2;
	}

	//---------------------------------------------------------------------------
	void CFrameModule::Update() {
		i32 old = _currentFrame - 2;
		if (old < 0)
			old += TAGS_MEMORY_COUNT;

		ReleaseFrameTags(_memoryModule, _frames + old);

		i32 nextToCreate = (_currentFrame + ALIVE_TAGS_COUNT - 1) % TAGS_MEMORY_COUNT;
		CreateFrameTags(_memoryModule, _frames + nextToCreate);

		++_currentFrame;
	}

	//---------------------------------------------------------------------------
	const Frame* CFrameModule::CurrentFrame() {
		return _frames + _currentFrame;
	}
}

