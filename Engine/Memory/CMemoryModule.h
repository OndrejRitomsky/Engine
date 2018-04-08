#pragma once

#include "Engine/Modules/ModuleState.h"

#include <Core/TagAllocator/MemTag.h>

namespace core {
	class IAllocator;
	class ITagAllocator;
}


namespace eng {
	struct Engine;

	class CStaticConstructor;
	class CHandleManager;

	class CMemoryModule {
	public:
		CMemoryModule();
		~CMemoryModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
		void Init(core::IAllocator* allocator);

		core::IAllocator* Allocator();
		core::ITagAllocator* TagAllocator();

		void MakeMemTag(u32 count, core::MemTag* tagsOut);
		void ReleaseMemTag(const core::MemTag* tags, u32 count);

	public:
		ModuleState _state;

		core::ITagAllocator* _taggedAllocator;
		core::IAllocator* _allocator;

		CHandleManager* _tagManager;
	};
}