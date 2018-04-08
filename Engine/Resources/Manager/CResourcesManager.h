#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/HashMap.h>

#include "Engine/Resources/Resource/Resource.h"

namespace core {
	class IAllocator;
}

namespace eng {
	struct ResourceLoadedEvents;
	struct ResourceLoadEvents;
	struct ResourceReadyEvents;

	class CResourcesManager {
	private:
		struct LoadData;
		struct ResourceData;

	public:
		CResourcesManager();
		~CResourcesManager();

		void Init(core::IAllocator* allocator);

		void OnResourceReady(const ResourceReadyEvents* evt);

		void HasResources(const h64* hashes, u32 count, bool* outHasResources);
		void AcquireResource(const h64* hashes, u64 count, Resource* outResources);
		void ReleaseResource(const h64* hashes, u64 count); // we need release with resource // so reverse map

	private:
		core::HashMap<ResourceData> _resourceMap;
	};
}

