#include "resources_manager.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/collection/array.inl>
#include <core/allocator/allocate.h>

#include "../resource_internal_event.h"
#include "../resource_event.h"

#include "../../frame/frame.h"

namespace eng {

	struct ResourcesManager::ResourceData {
		Resource resource;
	};

	enum class LoadStatus : u8 {
		REQUESTED,
		LOADING,
		LOADED,
		EVENTS_SENT
	};

	struct ResourcesManager::LoadData {
		LoadStatus status;
		i32 requestsCount;
		u64 typeID;
	};

	struct ResourcesManager::LoadRequest {
		h64 hash;
		u64 typeID;
	};


	ResourcesManager::ResourcesManager() :
		_allocator(nullptr),
		_tagAllocator(nullptr) {
		_loadEvents = {0};
		_loadedEvents = {0};
	}


	ResourcesManager::~ResourcesManager() {
		//	ASSERT(_loadMap.Count() == 0);
	}


	void ResourcesManager::ClearPendingEvents() {
		_loadEvents.capacity = 0;
		_loadEvents.count = 0;
		_loadEvents.typesID = nullptr;
		_loadEvents.hashes = nullptr;

		_loadedEvents.count = 0;
		_loadedEvents.capacity = 0;
		_loadedEvents.hashes = nullptr;
	}

	void ResourcesManager::Update(const Frame* frame) {

		{ // check new requests
			for (const LoadRequest& req : _loadRequests) {
				LoadData* loadData = _loadMap.Find(req.hash);
				if (loadData) {
					++loadData->requestsCount;
				}
				else {
					LoadData data;
					data.status = _resourceMap.Find(req.hash) ? LoadStatus::LOADED : LoadStatus::REQUESTED;
					data.requestsCount = 1;
					data.typeID = req.typeID;
					_loadMap.Add(req.hash, data);
				}
			}

			_loadRequests.Clear();
		}

		{ // Iterating and deleting from load map // @TODO could be merged
			const auto it = _loadMap.CIterator();
			u32 count = it.count;
			u32 i = 0;
			while (i < count) {
				if (it.values[i].requestsCount == 0 || it.values[i].status == LoadStatus::EVENTS_SENT) {
				//if (!it.values[i].requestsCount && it.values[i].status != LoadStatus::LOADING) {
					_loadMap.SwapRemove(it.keys[i]);
					--count;
				}
				else {
					++i;
				}
			}
		} 



		{ // Update new requests
			auto it = _loadMap.Iterator();

			if (it.count > 0) {
				AllocateLoadEventsBuffer(frame, it.count);
				AllocateLoadedEventsBuffer(frame, it.count);
			}

			u32 i = 0;
			while (i < it.count) {
				LoadData* loadData = it.values + i;
				h64 key = it.keys[i];

				if (loadData->requestsCount > 0) {
					if (loadData->status == LoadStatus::REQUESTED) {
						_loadEvents.hashes[_loadEvents.count] = key;
						_loadEvents.typesID[_loadEvents.count] = loadData->typeID;
						++_loadEvents.count;

						loadData->status = LoadStatus::LOADING;
					}
					else if (loadData->status == LoadStatus::LOADED) {
						_loadedEvents.hashes[_loadedEvents.count] = key;
						++_loadedEvents.count;

						loadData->status = LoadStatus::EVENTS_SENT;
					}
				}
				++i;
			}
		}
	}


	void ResourcesManager::Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator) {
		_allocator = allocator;
		_tagAllocator = tagAllocator;

		_resourceMap.Init(allocator);
		_loadMap.Init(allocator);
		_loadRequests.Init(allocator);
	}


	void ResourcesManager::OnResourceReady(const ResourceReadyEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			LoadData* loadData = _loadMap.Find(evt->hashes[i]);
			ASSERT(loadData); // it had to requested
			loadData->status = LoadStatus::LOADED;

			ResourceData res = {evt->resources[i]};
			_resourceMap.Add(evt->hashes[i], res);
		}	
	}

	void ResourcesManager::HasResources(const h64* hashes, u32 count, bool* outHasResources) {
		for (u64 i = 0; i < count; ++i) {
			ResourceData* data = _resourceMap.Find(hashes[i]);
			outHasResources[i] = data != nullptr;
		}
	}

	void ResourcesManager::AcquireResource(const h64* hashes, u64 count, Resource* outResources) {
		for (u64 i = 0; i < count; ++i) {
			ResourceData* data = _resourceMap.Find(hashes[i]);
			ASSERT(data);

			outResources[i] = data->resource;
			// RESOURCE COUNTING
		}
	}


	void ResourcesManager::ReleaseResource(const h64* hashes, u64 count) {
		for (u64 i = 0; i < count; ++i) {

		}
	}


	void ResourcesManager::GetLoadEvents(ResourceLoadEvents* outEvents) {
		outEvents->count = _loadEvents.count;
		outEvents->hashes = _loadEvents.hashes;
		outEvents->typeIDs = _loadEvents.typesID;
	}

	void ResourcesManager::GetLoadedEvents(ResourceLoadedEvents* outEvents) {
		outEvents->count = _loadedEvents.count;
		outEvents->hashes = _loadedEvents.hashes;
	}

	void ResourcesManager::RequestLoad(const h64* hashes, u32 count, const u64* typesID) {
		for (u32 i = 0; i < count; ++i) {
			LoadRequest req;
			req.hash = hashes[i];
			req.typeID = typesID[i];
			_loadRequests.Push(req);

			ASSERT(!_resourceMap.Find(hashes[i]));
		}
	}

	void ResourcesManager::RequestLoad(const h64* hashes, u32 count, u64 typeID) {
		for (u32 i = 0; i < count; ++i) {
			LoadRequest req;
			req.hash = hashes[i];
			req.typeID = typeID;
			_loadRequests.Push(req);

			ASSERT(!_resourceMap.Find(hashes[i]));
		}
	}


	/*void ResourcesManager::CancelLoad(const h64* hashes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			LoadData* loadData = _loadMap.Find(hashes[i]);
			ASSERT(loadData);
			ASSERT(loadData->requestsCount > 0);
			--loadData->requestsCount;
		}
	}


	void ResourcesManager::AcknowledgeLoaded(const h64* hashes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			LoadData* loadData = _loadMap.Find(hashes[i]);
			ASSERT(loadData);
			ASSERT(loadData->requestsCount > 0);
			ASSERT(loadData->status == LoadStatus::LOADED);
			--loadData->requestsCount;
		}
	}*/


	void ResourcesManager::AllocateLoadEventsBuffer(const Frame* frame, u64 capacity) {
		const u64 ELEMENTS_SIZE = (sizeof(h64) + sizeof(u64));
		const u64 EXTRA_SIZE = alignof(u64);

		u64 size = capacity * ELEMENTS_SIZE + EXTRA_SIZE;

		h64* hashes = (h64*) TagAllocate(_tagAllocator, frame->gameTag, size, alignof(h64));

		_loadEvents.capacity = (u32) capacity;
		_loadEvents.count = 0;
		_loadEvents.typesID = static_cast<u64*>(core::PointerAlign(&hashes[capacity], alignof(u64)));
		_loadEvents.hashes = hashes;
	}

	void ResourcesManager::AllocateLoadedEventsBuffer(const Frame* frame, u64 capacity) {
		u64 size = capacity * sizeof(h64);

		_loadedEvents.hashes = (h64*) TagAllocate(_tagAllocator, frame->gameTag, size, alignof(h64));
		_loadedEvents.capacity = (u32) capacity;
		_loadedEvents.count = 0;
	}
}