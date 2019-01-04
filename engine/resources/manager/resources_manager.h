#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/array.h>

#include "../resource/resource.h"

namespace core {
	struct ITagAllocator;
	struct IAllocator;
}
namespace eng {
	struct ResourceLoadedEvents;
	struct ResourceLoadEvents;
	struct ResourceReadyEvents;
	struct ResourcePreloadEvents;
	struct Frame;

	class ResourcesManager {
	private:
		struct LoadData;
		struct ResourceData;

		struct LoadRequest;

	public:
		ResourcesManager();
		~ResourcesManager();

		void Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator);
		void Update(const Frame* frame);
		void ClearPendingEvents();

		void OnResourceReady(const ResourceReadyEvents* evt);
		void OnResourcePreload(const ResourcePreloadEvents* evt); // DEBUG

		void GetLoadEvents(ResourceLoadEvents* outEvents);
		void GetLoadedEvents(ResourceLoadedEvents* outEvents);

		void HasResources(const h64* hashes, u32 count, bool* outHasResources);
		void AcquireResource(const h64* hashes, u64 count, Resource* outResources);
		void ReleaseResource(const h64* hashes, u64 count); // we need release with resource // so reverse map

		void RequestLoad(const h64* hashes, u32 count, const u64* typesID);

		/*void CancelLoad(const h64* hashes, u32 count);

		void AcknowledgeLoaded(const h64* hashes, u32 count);*/

	private:
		void AllocateLoadEventsBuffer(const Frame* frame, u64 capacity);
		void AllocateLoadedEventsBuffer(const Frame* frame, u64 capacity);

	private:
		struct LoadEvents {
			u32 count;
			u32 capacity;
			u64* typesID;
			h64* hashes;
		};
		
		struct LoadedEvents {
			u32 count;
			u32 capacity;
			h64* hashes;
		};
		
		LoadEvents _loadEvents;
		LoadedEvents _loadedEvents;

		core::Array<LoadRequest> _loadRequests;
		core::HashMap<LoadData> _loadMap;
		core::HashMap<ResourceData> _resourceMap;

		core::IAllocator* _allocator;
		core::ITagAllocator* _tagAllocator;
	};
}

