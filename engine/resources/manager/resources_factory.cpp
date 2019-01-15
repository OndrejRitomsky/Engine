#include "resources_factory.h"

#include <core/common/debug.h>
#include <core/common/pointer.h>
#include <core/collection/hashmap.inl>
#include <core/collection/lookuparray.inl>
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

	struct ResourcesFactory::PendingPackage {
		DependencyPackageStatus status;
		core::Handle handle;
	};

	ResourcesFactory::ResourcesFactory() {
	}

	ResourcesFactory::~ResourcesFactory() {
	}

	void ResourcesFactory::Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator) {
		_tagAllocator = tagAllocator;

		_packages.Init(allocator, 128);

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
		// Update dependency packages (O(k*N) (dependency count))
		for (auto& package : _packageMap) {
			if (package.status == DependencyPackageStatus::NEW) {
				UpdateNewPackage(&package, frame);
			}
			else if (package.status == DependencyPackageStatus::PENDING) {
				UpdatePendingPackage(&package);
			}
			else if (package.status == DependencyPackageStatus::DELETE) {
				++deleteCount;
			}

			if (package.status == DependencyPackageStatus::READY)
				UpdateReadyPackage(&package, frame);
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
		// @TODO its not the nicest to do this directly in event? but makes the delay smaller

		for (u32 i = 0; i < evt->count; ++i) {
			ASSERT(!_packageMap.Find(evt->hashes[i]));

			IResourceTypeFactory* constructor = TypeIDToResourceConstructor(evt->typeIDs[i]);

			ResourcePackage package;
			package.dependant.typeID   = evt->typeIDs[i];
			package.dependant.hash     = evt->hashes[i];
			package.dependant.resource = INVALID_RESOURCE_VALUE;

			constructor->TryCreate(constructor, evt->buffers[i], (u32) evt->bufferSizes[i], &package);

			PendingPackage pp;
			pp.status = package.dependenciesCount == 0 ? DependencyPackageStatus::READY : DependencyPackageStatus::NEW;
			pp.handle = _packages.Add(package);

			_packageMap.Add(evt->hashes[i], pp); // what if it exist?
		}
	}

	void ResourcesFactory::UpdateNewPackage(PendingPackage* pp, const Frame* frame) {
		ASSERT(pp->status == DependencyPackageStatus::NEW);
		Log("New package frame: %llu\n", frame->frameIndex);

		ResourcePackage* package = &_packages.Get(pp->handle);

		if (_loadEvents.count + package->dependenciesCount > _loadEvents.capacity) {
			ReallocateLoadEventsBuffer(frame);
			ASSERT(_loadEvents.count + package->dependenciesCount <= _loadEvents.capacity);
		}

		u32 readyCount = 0;
		for (u32 i = 0; i < package->dependenciesCount; ++i) {
			ResourceDescription* dependency = &package->dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dependency->hash);

			if (it) {
				if (it->status == PendingDependencyStatus::READY) {
					dependency->resource = it->resource;
					++readyCount;
				}
				++it->dependantsCount; // Its dependant even if its ready
			}
			else {
				dependency->resource = INVALID_RESOURCE_VALUE;

				PendingDependency newDep;
				newDep.dependantsCount = 1;
				newDep.resource = INVALID_RESOURCE_VALUE;
				newDep.status = PendingDependencyStatus::PENDING;
				_dependencyMap.Add(dependency->hash, newDep);

				_loadEvents.typesID[_loadEvents.count] = dependency->typeID;
				_loadEvents.hashes[_loadEvents.count] = dependency->hash;
				++_loadEvents.count;
			}
		}

		pp->status = readyCount == package->dependenciesCount ? DependencyPackageStatus::READY : DependencyPackageStatus::PENDING;
	}

	void ResourcesFactory::UpdatePendingPackage(PendingPackage* pp) {
		ASSERT(pp->status == DependencyPackageStatus::PENDING);

		ResourcePackage* package = &_packages.Get(pp->handle);

		u32 readyCount = 0;
		for (u32 i = 0; i < package->dependenciesCount; ++i) {
			ResourceDescription* dependency = &package->dependencies[i];

			if (dependency->resource == INVALID_RESOURCE_VALUE) {
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

		if (readyCount == package->dependenciesCount)
			pp->status = DependencyPackageStatus::READY;
	}

	void ResourcesFactory::UpdateReadyPackage(PendingPackage* pp, const Frame* frame) {
		ASSERT(pp->status == DependencyPackageStatus::READY);

		Log("Ready package frame: %llu\n", frame->frameIndex);

		ResourcePackage* package = &_packages.Get(pp->handle);

		if (package->dependant.resource == INVALID_RESOURCE_VALUE) {
			ASSERT(package->dependenciesCount > 0);

			IResourceTypeFactory* factory = TypeIDToResourceConstructor(package->dependant.typeID);
			factory->Create(factory, package);

			for (u32 i = 0; i < package->dependenciesCount; ++i) {
				ResourceDescription* dependency = &package->dependencies[i];
				PendingDependency* it = _dependencyMap.Find(dependency->hash);
				ASSERT(it); // has to exists
				--it->dependantsCount;
			}
		}
		else {
			ASSERT(package->dependenciesCount == 0);
		}
		
		if (_readyEvents.count >= _readyEvents.capacity) {
			ReallocateFinishedEventsBuffer(frame);
			ASSERT(_readyEvents.count + 1 <= _readyEvents.capacity);
		}

		_readyEvents.hashes[_readyEvents.count] = package->dependant.hash;
		_readyEvents.resources[_readyEvents.count] = package->dependant.resource;

		++_readyEvents.count;
		pp->status = DependencyPackageStatus::DELETE;
	}

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