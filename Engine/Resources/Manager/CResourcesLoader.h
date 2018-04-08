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
	struct ResourcePreloadEvents;

	class CResourcesLoader {
	private:
		struct LoadData;

	public:
		CResourcesLoader();
		~CResourcesLoader();

		void Init(core::IAllocator* allocator);
		void Update();
		void ClearPendingEvents();

		void OnResourceReady(const ResourceReadyEvents* evt);
		void OnResourcePreload(const ResourcePreloadEvents* evt); // DEBUG

		void GetLoadEvents(ResourceLoadEvents* outEvents);
		void GetLoadedEvents(ResourceLoadedEvents* outEvents);

		// API
		void RequestLoad(const h64* hashes, u32 count, const u64* typesID);
		void CancelLoad(const h64* hashes, u32 count);
		void AcknowledgeLoaded(const h64* hashes, u32 count);
		// API

	private:

		void DeleteHangingRequests();
		void CreateLoadedEvents();
		void UpdateNewRequests();

	private:
		static const u32 LOAD_CAPACITY = 20u;
		static const u32 LOADED_CAPACITY = 20u;

		// @TODO proper buffers

		u32 _loadCount;
		h64 _loadHashes[LOAD_CAPACITY];
		u64 _loadTypes[LOAD_CAPACITY];

		u32 _loadedCount;
		h64 _loadedHashes[LOADED_CAPACITY];

		core::HashMap<LoadData> _loadMap;
	};
}

