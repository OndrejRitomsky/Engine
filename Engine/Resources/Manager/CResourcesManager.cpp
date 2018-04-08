#include "CResourcesManager.h"

#include <Core/Common/Assert.h>
#include <Core/Collection/HashMap.inl>

#include "Engine/Resources/ResourceInternalEvent.h"

namespace eng {

	struct CResourcesManager::ResourceData {
		Resource resource;
	};

	//---------------------------------------------------------------------------
	CResourcesManager::CResourcesManager() {
	}

	//---------------------------------------------------------------------------
	CResourcesManager::~CResourcesManager() {
	}

	//---------------------------------------------------------------------------
	void CResourcesManager::Init(core::IAllocator* allocator) {
		_resourceMap.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CResourcesManager::OnResourceReady(const ResourceReadyEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			ResourceData res = {evt->resources[i]};
			_resourceMap.Add(evt->hashes[i], res);
		}	
	}

	//---------------------------------------------------------------------------
	void CResourcesManager::HasResources(const h64* hashes, u32 count, bool* outHasResources) {
		for (u64 i = 0; i < count; ++i) {
			ResourceData* data = _resourceMap.Find(hashes[i]);
			outHasResources[i] = data != nullptr;
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesManager::AcquireResource(const h64* hashes, u64 count, Resource* outResources) {
		for (u64 i = 0; i < count; ++i) {
			ResourceData* data = _resourceMap.Find(hashes[i]);
			ASSERT(data);

			outResources[i] = data->resource;
			// RESOURCE COUNTING
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesManager::ReleaseResource(const h64* hashes, u64 count) {
		for (u64 i = 0; i < count; ++i) {

		}
	}

}