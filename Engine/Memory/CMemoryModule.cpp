#include "CMemoryModule.h"

#include <Core/Common/Assert.h>
#include <Core/Allocator/IAllocator.h>
#include <Core/Allocator/IAllocator.inl>

#include <Core/TagAllocator/ITagAllocator.h>
#include <Core/TagAllocator/ITagAllocator.inl>

#include <Core/TagAllocator/SafeTaggedBlockAllocator.h>

#include "Engine/Util/CHandleManager.h"

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

namespace eng {
	//---------------------------------------------------------------------------
	CMemoryModule::CMemoryModule() :
		_state(ModuleState::CREATED),
		_taggedAllocator(nullptr),
		_allocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CMemoryModule::~CMemoryModule() {
		_tagManager->~CHandleManager();

		_taggedAllocator->Deinit();
	}

	//---------------------------------------------------------------------------
	ModuleState CMemoryModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CMemoryModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_taggedAllocator = constructor->Construct<core::SafeTaggedBlockAllocator>();
		_tagManager = constructor->Construct<CHandleManager>();
	}

	//---------------------------------------------------------------------------
	void CMemoryModule::Init(core::IAllocator* allocator) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		_allocator = allocator;
		reinterpret_cast<core::SafeTaggedBlockAllocator*>(_taggedAllocator)->Init(allocator, 2 * 1024 * 1024, 64);
		_tagManager->Init(_allocator);
	}

	//---------------------------------------------------------------------------
	core::IAllocator* CMemoryModule::Allocator() {
		return _allocator;
	}

	//---------------------------------------------------------------------------
	core::ITagAllocator* CMemoryModule::TagAllocator() {
		return _taggedAllocator;
	}

	//---------------------------------------------------------------------------
	void CMemoryModule::MakeMemTag(u32 count, core::MemTag* tagsOut) {
		for (u32 i = 0; i < count; ++i) {
			tagsOut[i] = _tagManager->NewHandle();
		}
	}

	//---------------------------------------------------------------------------
	void CMemoryModule::ReleaseMemTag(const core::MemTag* tags, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			u32 handle = static_cast<u32>(tags[i]);
			_tagManager->ReturnHandle(handle);
		}
	}

}
