#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/lookuparray.h>

#include "../resource.h"

namespace core {
	struct ITagAllocator;
	struct IAllocator;
}

namespace eng {
	struct IResourceTypeFactory;

	struct Frame;
	struct ResourceDependencyLoadEvents;
	struct ResourceDataLoadedEvents;
	struct ResourceReadyEvents;

	struct ResourcePackage;

	class ResourcesFactory {
	private:
		struct PendingPackage;
		struct PendingDependency;

	public:
		ResourcesFactory();
		~ResourcesFactory();

		void Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator);
		void Update(const Frame* frame);

		void RegisterTypeConstructor(u64 typeID, IResourceTypeFactory* constructor);

		void ClearPendingEvents();

		void GetLoadEvents(ResourceDependencyLoadEvents* evt);
		void GetReadyEvents(ResourceReadyEvents* evt);

		void OnDependencyReady(const ResourceReadyEvents* evt);
		void OnNewResource(const ResourceDataLoadedEvents* evt);

	private:
		IResourceTypeFactory* TypeIDToResourceConstructor(u64 typeID);

		void UpdateNewPackage(PendingPackage* pp, const Frame* frame);
		void UpdatePendingPackage(PendingPackage* pp);
		void UpdateReadyPackage(PendingPackage* pp, const Frame* frame);

		void ReallocateLoadEventsBuffer(const Frame* frame);
		void ReallocateFinishedEventsBuffer(const Frame* frame);

	private:

		struct LoadEvents {
			u32 count = 0;
			u32 capacity = 0;
			u64* typesID  = nullptr;
			h64* hashes = nullptr;
		};

		struct ReadyEvents {
			u32 count = 0;
			u32 capacity = 0;

			h64* hashes = nullptr;
			Resource* resources = nullptr;
		};

		LoadEvents _loadEvents;
		ReadyEvents _readyEvents; // @RENAME CONSTRUCTED

		core::ITagAllocator* _tagAllocator = nullptr;

		core::LookupArray<ResourcePackage> _packages;

		core::HashMap<PendingDependency> _dependencyMap;
		core::HashMap<PendingPackage> _packageMap;

		core::HashMap<IResourceTypeFactory*> _constructorMap;
	};
}