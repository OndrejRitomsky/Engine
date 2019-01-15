#include "memory_module.h"

#include <core/common/debug.h>
#include <core/allocator/allocate.h>
#include <core/allocator/allocators.h>

#include "../Engine.h"

#include "../util/handle_manager.h"

#include "permanent_allocator.h"

namespace eng {
	MemoryModule::MemoryModule() :
		_state(ModuleState::CREATED),
		_taggedAllocator(nullptr),
		_allocator(nullptr) {
	}

	MemoryModule::~MemoryModule() {
		_tagManager->~HandleManager();

		TagAllocatorDeinit(_taggedAllocator);
	}


	ModuleState MemoryModule::State() {
		return _state;
	}

	void MemoryModule::ConstructSubsytems(PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_taggedAllocator = &(PermanentNew(permanentAllocator, core::SafeTaggedBlockAllocator)())->allocator;
		_tagManager = PermanentNew(permanentAllocator, HandleManager)();
	}

	void MemoryModule::Init(core::IAllocator* allocator, PermanentAllocator* permanentAllocator) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		_allocator = allocator;
		core::SafeTaggedBlockAllocatorInit((core::SafeTaggedBlockAllocator*) _taggedAllocator, allocator, 2 * 1024 * 1024, 128);

		_tagManager->Init(_allocator);
		_permanentAllocator = permanentAllocator;
	}

	core::IAllocator* MemoryModule::Allocator() {
		return _allocator;
	}

	core::ITagAllocator* MemoryModule::TagAllocator() {
		return _taggedAllocator;
	}

	PermanentAllocator* MemoryModule::PermaAllocator() {
		return _permanentAllocator;
	}

	void MemoryModule::MakeMemTag(u32 count, core::MemTag* tagsOut) {
		for (u32 i = 0; i < count; ++i) {
			tagsOut[i] = _tagManager->NewHandle();
		}
	}

	void MemoryModule::ReleaseMemTag(const core::MemTag* tags, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			u32 handle = static_cast<u32>(tags[i]);
			_tagManager->ReturnHandle(handle);
		}
	}
}
