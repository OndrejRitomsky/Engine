#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/HashMap.h>

#include "Engine/Resources/Resource/Resource.h"

namespace core {
	class ITagAllocator;
	class IAllocator;
}

namespace eng {
	class IResourceConstructor;

	struct Frame;
	struct ResourceDependencyLoadEvents;
	struct ResourceDataLoadedEvents;
	struct ResourceReadyEvents;

	class CResourcesFactory {
	private:
		struct DependencyPackage;
		struct PendingDependency;

	public:
		CResourcesFactory();
		~CResourcesFactory();

		void Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator);
		void Update(const Frame* frame);

		void RegisterTypeConstructor(u64 typeID, IResourceConstructor* constructor);

		void ClearPendingEvents();

		void GetLoadEvents(ResourceDependencyLoadEvents* evt);
		void GetReadyEvents(ResourceReadyEvents* evt);

		void OnDependencyReady(const ResourceReadyEvents* evt);
		void OnNewResource(const ResourceDataLoadedEvents* evt);

	private:
		IResourceConstructor* TypeIDToResourceConstructor(u64 typeID);

		void UpdateNewPackage(DependencyPackage* package, const Frame* frame);
		void UpdatePendingPackage(DependencyPackage* package);
		void UpdateReadyPackage(DependencyPackage* package, const Frame* frame);
		void UpdateEventsSentPackage(DependencyPackage* package, const Frame* frame);

		void ReallocateLoadEventsBuffer(const Frame* frame);
		void ReallocateFinishedEventsBuffer(const Frame* frame);

	private:

		struct LoadEvents {
			u32 count;
			u32 capacity;
			u64* typesID;
			h64* hashes;
		};

		struct ReadyEvents {
			u32 count;
			u32 capacity;

			h64* hashes;
			Resource* resources;
		};

		LoadEvents _loadEvents;
		ReadyEvents _readyEvents; // @RENAME CONSTRUCTED

		core::IAllocator* _allocator;
		core::ITagAllocator* _tagAllocator;

		core::HashMap<PendingDependency> _dependencyMap;
		core::HashMap<DependencyPackage> _packageMap;

		core::HashMap<IResourceConstructor*> _constructorMap;
	};
}