#include "CResourcesLoader.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/HashMap.inl>
#include <Core/Allocator/IAllocator.h>

#include "Engine/Resources/ResourceEvent.h"

#include "Engine/Resources/ResourceInternalEvent.h"

namespace eng {
	enum class LoadStatus : u8 {
		REQUESTED,
		LOADING,
		LOADED,
		EVENTS_SENT
	};

	struct CResourcesLoader::LoadData {
		LoadStatus status;
		i32 requestsCount;
		u64 typeID;
	};
	//---------------------------------------------------------------------------
	CResourcesLoader::CResourcesLoader() :
		_loadCount(0),
		_loadedCount(0) {
	}

	//---------------------------------------------------------------------------
	CResourcesLoader::~CResourcesLoader() {
	//	ASSERT(_loadMap.Count() == 0);
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::Init(core::IAllocator* allocator) {
		_loadMap.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::Update() {
		// Iterating and deleting from load map
		DeleteHangingRequests();

		// iterating load map
		UpdateNewRequests();

		// iterating load map
		CreateLoadedEvents();
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::ClearPendingEvents() {
		_loadCount = 0;
		_loadedCount = 0;
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::OnResourceReady(const ResourceReadyEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			LoadData* loadData = _loadMap.Find(evt->hashes[i]);
			ASSERT(loadData); // it had to requested
			loadData->status = LoadStatus::LOADED;
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::OnResourcePreload(const ResourcePreloadEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			LoadData data;
			data.status = LoadStatus::LOADING;
			data.requestsCount = 1;
			data.typeID = evt->typeIDs[i];
			_loadMap.Add(evt->hashes[i], data);
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::GetLoadEvents(ResourceLoadEvents* outEvents) {
		outEvents->count = _loadCount;
		outEvents->hashes = _loadHashes;
		outEvents->typeIDs = _loadTypes;
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::GetLoadedEvents(ResourceLoadedEvents* outEvents) {
		outEvents->count = _loadedCount;
		outEvents->hashes = _loadedHashes;
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::DeleteHangingRequests() {
		const auto it = _loadMap.CIterator();
		u32 count = it.count;
		u32 i = 0;
		while (i < count) {
			if (!it.values[i].requestsCount && it.values[i].status != LoadStatus::LOADING) {
				_loadMap.SwapRemove(it.keys[i]);
				--count;
			}
			else {
				++i;
			}
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::CreateLoadedEvents() {
		auto it = _loadMap.Iterator();
		u32 i = 0;
		while (i < it.count && _loadedCount < LOADED_CAPACITY) {
			if (it.values[i].status == LoadStatus::LOADED && it.values[i].requestsCount > 0) {
				_loadedHashes[_loadedCount] = it.keys[i];

				it.values[i].status = LoadStatus::EVENTS_SENT;
				++_loadedCount;
			}
			++i;
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::UpdateNewRequests() {
		auto it = _loadMap.Iterator();
		u32 i = 0;

		while (i < it.count && _loadCount < LOAD_CAPACITY) {
			if (it.values[i].status == LoadStatus::REQUESTED && it.values[i].requestsCount > 0) {
				_loadHashes[_loadCount] = it.keys[i];
				_loadTypes[_loadCount]  = it.values[i].typeID;
				it.values[i].status = LoadStatus::LOADING;
				++_loadCount;
			}
			++i;
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::RequestLoad(const h64* hashes, u32 count, const u64* typesID) {
		for (u32 i = 0; i < count; ++i) {
			LoadData* loadData = _loadMap.Find(hashes[i]);
			if (loadData) {
				++loadData->requestsCount;
			}
			else {
				LoadData data;
				data.status = LoadStatus::REQUESTED;
				data.requestsCount = 1;
				data.typeID = typesID[i];
				_loadMap.Add(hashes[i], data);
			}
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::CancelLoad(const h64* hashes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			LoadData* loadData = _loadMap.Find(hashes[i]);
			ASSERT(loadData);
			ASSERT(loadData->requestsCount > 0);
			--loadData->requestsCount;
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesLoader::AcknowledgeLoaded(const h64* hashes, u32 count) {
		for (u32 i = 0; i < count; ++i) {
			LoadData* loadData = _loadMap.Find(hashes[i]);
			ASSERT(loadData);
			ASSERT(loadData->requestsCount > 0);
			ASSERT(loadData->status == LoadStatus::LOADED);
			--loadData->requestsCount;
		}
	}
}