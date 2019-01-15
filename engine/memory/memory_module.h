#pragma once

#include "../module.h"


namespace core {
	struct IAllocator;
	struct ITagAllocator;
}


namespace eng {
	struct Engine;
	struct PermanentAllocator;

	class HandleManager;

	class MemoryModule {
	public:
		MemoryModule();
		~MemoryModule();

		ModuleState State();
		void ConstructSubsytems(PermanentAllocator* permanentAllocator);
		void Init(core::IAllocator* allocator, PermanentAllocator* permanentAllocator);

		core::IAllocator* Allocator();
		core::ITagAllocator* TagAllocator();
		PermanentAllocator* PermaAllocator();

		void MakeMemTag(u32 count, core::MemTag* tagsOut);
		void ReleaseMemTag(const core::MemTag* tags, u32 count);

	private:
		ModuleState _state;

		core::ITagAllocator* _taggedAllocator;
		core::IAllocator* _allocator;
		
		PermanentAllocator* _permanentAllocator;
		HandleManager* _tagManager;
	};
}