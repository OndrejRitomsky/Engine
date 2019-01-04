#include "resources_factory.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/allocator/allocate.h>

#include "../resource_event.h"
#include "../resource_internal_event.h"

#include "../resource_manager/iresource_type_factory.h"

#include "../../frame/frame.h"

#include "../../common/log.h"

namespace eng {

	// @TODO MEMORY MANAGMENT

	enum class DependencyPackageStatus : u8 {
		NEW,
		PENDING,
		READY,
		EVENTS_SENT,
		DELETE
	};

	enum class PendingDependencyStatus : u8 {
		PENDING,
		READY
	};

	struct ResourcesFactory::PendingDependency {
		PendingDependencyStatus status;
		u32 dependantsCount;
		Resource resource;
	};

	struct ResourcesFactory::DependencyPackage {
		DependencyPackageStatus status;
		bool dependenciesReady[ResourceDependencies::DEPENDENCIES_CAPACITY];
		ResourceDependencies dependencies;
		u64 typeID;
	};



	ResourcesFactory::ResourcesFactory() :
		_allocator(nullptr),
		_tagAllocator(nullptr)
	{
		_loadEvents = {0};
		_readyEvents = {0};
	}

	ResourcesFactory::~ResourcesFactory() {
	}

	void ResourcesFactory::Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator) {
		_allocator = allocator;
		_tagAllocator = tagAllocator;

		_packageMap.Init(allocator);
		_dependencyMap.Init(allocator);
		_constructorMap.Init(allocator);
	}

	void ResourcesFactory::RegisterTypeConstructor(u64 typeID, IResourceTypeFactory* constructor) {
		ASSERT(!_constructorMap.Find(typeID));

		_constructorMap.Add(typeID, constructor);
	}

	IResourceTypeFactory* ResourcesFactory::TypeIDToResourceConstructor(u64 typeID) {
		auto it = _constructorMap.Find(typeID);
		ASSERT(it);
		return *it;
	}

	void ResourcesFactory::Update(const Frame* frame) {
		void* data = nullptr;

		ASSERT(!_loadEvents.count);

		u32 deleteCount = 0;
		{
			// Update dependency packages (O(k*N) (dependency count))
			for (auto& package : _packageMap) {
				switch (package.status) {
				case DependencyPackageStatus::NEW: UpdateNewPackage(&package, frame); break;
				case DependencyPackageStatus::PENDING: UpdatePendingPackage(&package); break;
					//case DependencyPackageStatus::READY: UpdateReadyPackage(&package, frame); break;
				case DependencyPackageStatus::EVENTS_SENT: ASSERT(false); break; //UpdateEventsSentPackage(&package, frame); break;
				case DependencyPackageStatus::DELETE: ++deleteCount;  break; // separate cycle
				default:
					ASSERT(false);
					break;
				}
			}

			// So package can jump from new / pending to ready if its dependencies are ready or it has none
			for (auto& package : _packageMap) {
				if (package.status == DependencyPackageStatus::READY)
					UpdateReadyPackage(&package, frame);
			}
		}

		if (deleteCount > 0) {
			// Delete finished dependency packages
			auto it = _packageMap.CIterator();
			u32 count = it.count;
			u32 i = 0;
			while (i < count) {
				if (it.values[i].status == DependencyPackageStatus::DELETE) {
					_packageMap.SwapRemove(it.keys[i]);
					--count;
				}
				else {
					++i;
				}
			}
		}

		{
			// Delete hanging dependencies
			auto it = _dependencyMap.CIterator();
			u32 count = it.count;
			u32 i = 0;
			while (i < count) {
				auto& dependency = it.values[i];
				if (dependency.status == PendingDependencyStatus::READY && !dependency.dependantsCount) {
					_dependencyMap.SwapRemove(it.keys[i]);
					--count;
				}
				else {
					++i;
				}
			}
		}

	}

	void ResourcesFactory::GetLoadEvents(ResourceDependencyLoadEvents* evt) {
		evt->count = _loadEvents.count;
		evt->typesID = _loadEvents.typesID;
		evt->hashes = _loadEvents.hashes;
	}

	void ResourcesFactory::GetReadyEvents(ResourceReadyEvents* evt) {
		evt->count = _readyEvents.count;
		evt->hashes = _readyEvents.hashes;
		evt->resources = _readyEvents.resources;
	}

	void ResourcesFactory::ClearPendingEvents() {
		_loadEvents.capacity = 0;
		_loadEvents.count = 0;
		_loadEvents.typesID = nullptr;
		_loadEvents.hashes = nullptr;

		_readyEvents.count = 0;
		_readyEvents.capacity = 0;
		_readyEvents.hashes = nullptr;
		_readyEvents.resources = nullptr;
	}

	void ResourcesFactory::OnDependencyReady(const ResourceReadyEvents* evt) {
		const h64* hashes = evt->hashes;
		const Resource* resources = evt->resources;
		
		for (u32 i = 0; i < evt->count; ++i) {
			PendingDependency* it = _dependencyMap.Find(evt->hashes[i]);
			// IT DOESNT HAVE TO EXIST
			if (it) {
				it->resource = evt->resources[i];
				it->status = PendingDependencyStatus::READY;
			}
		}
	}

	void ResourcesFactory::OnNewResource(const ResourceDataLoadedEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			ASSERT(!_packageMap.Find(evt->hashes[i]));


			IResourceTypeFactory* constructor = TypeIDToResourceConstructor(evt->typeIDs[i]);

			DependencyPackage package;
			package.typeID = evt->typeIDs[i];
			package.dependencies.dependant = evt->hashes[i];

			constructor->Parse(constructor, evt->buffers[i], &package.dependencies);
			package.status = DependencyPackageStatus::NEW;
			_packageMap.Add(evt->hashes[i], package); // what if it exist?
		}
	}

	void ResourcesFactory::UpdateNewPackage(DependencyPackage* package, const Frame* frame) {
		ASSERT(package->status == DependencyPackageStatus::NEW);


		Log("New package frame: %llu\n", frame->frameIndex);


		if (_loadEvents.count + package->dependencies.dependenciesCount > _loadEvents.capacity) {
			ReallocateLoadEventsBuffer(frame);
			ASSERT(_loadEvents.count + package->dependencies.dependenciesCount <= _loadEvents.capacity);
		}

		u32 readyCount = 0;
		for (u32 i = 0; i < package->dependencies.dependenciesCount; ++i) {
			ResourceDependency* dependency = &package->dependencies.dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dependency->hash);

			if (it) {
				if (it->status == PendingDependencyStatus::READY) {
					dependency->resource = it->resource;
					++readyCount;
				}
				package->dependenciesReady[i] = it->status == PendingDependencyStatus::READY;
				++it->dependantsCount; // Its dependant even if its ready
			}
			else {
				package->dependenciesReady[i] = false;

				PendingDependency newDep;
				newDep.dependantsCount = 1;
				newDep.resource = 0;
				newDep.status = PendingDependencyStatus::PENDING;
				_dependencyMap.Add(dependency->hash, newDep);

				_loadEvents.typesID[_loadEvents.count] = dependency->typeID;
				_loadEvents.hashes[_loadEvents.count] = dependency->hash;
				++_loadEvents.count;
			}
		}

		package->status = readyCount == package->dependencies.dependenciesCount ? DependencyPackageStatus::READY : DependencyPackageStatus::PENDING;
	}

	void ResourcesFactory::UpdatePendingPackage(DependencyPackage* package) {
		ASSERT(package->status == DependencyPackageStatus::PENDING);

		u32 readyCount = 0;
		for (u32 i = 0; i < package->dependencies.dependenciesCount; ++i) {
			ResourceDependency* dependency = &package->dependencies.dependencies[i];

			if (!package->dependenciesReady[i]) {
				PendingDependency* it = _dependencyMap.Find(dependency->hash);
				ASSERT(it); // has to exists
				if (it->status == PendingDependencyStatus::READY) {
					++readyCount;
					dependency->resource = it->resource;
				}
			}
			else {
				++readyCount;
			}
		}

		if (readyCount == package->dependencies.dependenciesCount)
			package->status = DependencyPackageStatus::READY;
	}

	void ResourcesFactory::UpdateReadyPackage(DependencyPackage* package, const Frame* frame) {
		ASSERT(package->status == DependencyPackageStatus::READY);
		if (_readyEvents.count >= _readyEvents.capacity) {
			ReallocateFinishedEventsBuffer(frame);
			ASSERT(_readyEvents.count + package->dependencies.dependenciesCount <= _readyEvents.capacity);
		}

		Log("Ready package frame: %llu\n", frame->frameIndex);

		IResourceTypeFactory* factory = TypeIDToResourceConstructor(package->typeID);
		Resource resource = factory->Create(factory, &package->dependencies);

		_readyEvents.hashes[_readyEvents.count] = package->dependencies.dependant;
		_readyEvents.resources[_readyEvents.count] = resource;

		++_readyEvents.count;
		package->status = DependencyPackageStatus::DELETE;

		for (u32 i = 0; i < package->dependencies.dependenciesCount; ++i) {
			ResourceDependency* dependency = &package->dependencies.dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dependency->hash);
			ASSERT(it); // has to exists
			--it->dependantsCount;
		}
	}

	//void ResourcesFactory::UpdateEventsSentPackage(DependencyPackage* package, const Frame* frame) {
	//	ASSERT(false);
	//		ASSERT(package->status == DependencyPackageStatus::EVENTS_SENT);
	//
	//	for (u32 i = 0; i < package->dependencies.dependenciesCount; ++i) {
	//		ResourceDependency* dependency = &package->dependencies.dependencies[i];
	//		PendingDependency* it = _dependencyMap.Find(dependency->hash);
	//		ASSERT(it); // has to exists
	//		--it->dependantsCount;
	//	}
	//
	//	package->status = DependencyPackageStatus::DELETE;
	//}

	void ResourcesFactory::ReallocateLoadEventsBuffer(const Frame* frame) {
		h64* hashes;
		u64* typesID;

		const u64 ELEMENTS_SIZE = (sizeof(h64) + sizeof(u64));
		const u64 EXTRA_SIZE = alignof(u64);
		u64 size = (_loadEvents.capacity ? 2 * _loadEvents.capacity : 128) * ELEMENTS_SIZE + EXTRA_SIZE;

		u64 allocated;
		hashes = (h64*) TagAllocate(_tagAllocator, frame->gameTag, size, alignof(h64), &allocated);
		u32 capacity = static_cast<u32>((allocated - EXTRA_SIZE) / ELEMENTS_SIZE);

		typesID = static_cast<u64*>(core::PointerAlign(&hashes[capacity], alignof(u64)));

		LoadEvents oldLoadEvents = _loadEvents;

		_loadEvents.capacity = capacity;
		_loadEvents.count = oldLoadEvents.count;
		_loadEvents.typesID = typesID;
		_loadEvents.hashes = hashes;

		if (oldLoadEvents.typesID) {
			core::Memcpy(_loadEvents.hashes, oldLoadEvents.hashes, oldLoadEvents.count * sizeof(h64));
			core::Memcpy(_loadEvents.typesID, oldLoadEvents.typesID, oldLoadEvents.count * sizeof(u64));
		}
	}

	void ResourcesFactory::ReallocateFinishedEventsBuffer(const Frame* frame) {
		h64* hashes;
		Resource* resources;

		const u64 ELEMENTS_SIZE = (sizeof(h64) + sizeof(Resource));
		const u64 EXTRA_SIZE = alignof(Resource);
		u64 size = (_readyEvents.capacity ? 2 * _readyEvents.capacity : 128) * ELEMENTS_SIZE + EXTRA_SIZE;

		u64 allocated;
		hashes = (h64*) TagAllocate(_tagAllocator, frame->gameTag, size, alignof(h64), &allocated);
		u32 capacity = static_cast<u32>((allocated - EXTRA_SIZE) / ELEMENTS_SIZE);

		resources = static_cast<Resource*>(core::PointerAlign(&hashes[capacity], alignof(Resource)));

		ReadyEvents oldFinishedEvents = _readyEvents;

		_readyEvents.count = oldFinishedEvents.count;
		_readyEvents.capacity = capacity;
		_readyEvents.hashes = hashes;
		_readyEvents.resources = resources;

		if (oldFinishedEvents.count) {
			core::Memcpy(_readyEvents.hashes, _readyEvents.hashes, _readyEvents.count * sizeof(h64));
			core::Memcpy(_readyEvents.resources, _readyEvents.resources, _readyEvents.count * sizeof(Resource));
		}
	}

}