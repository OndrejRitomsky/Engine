#include "CResourcesFactory.h"

#include <Core/Common/Assert.h>
#include <Core/Common/Pointer.h>
#include <Core/Collection/HashMap.inl>
#include <Core/Allocator/IAllocator.h>
#include <Core/TagAllocator/ITagAllocator.h>
#include <Core/TagAllocator/ITagAllocator.inl>

#include "Engine/Resources/ResourceEvent.h"
#include "Engine/Resources/ResourceInternalEvent.h"

#include "Engine/Resources/ResourceManager/IResourceConstructor.h"

#include "Engine/Frame/Frame.h"

namespace eng {

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

	struct CResourcesFactory::PendingDependency {
		PendingDependencyStatus status;
		u32 dependantsCount;
		Resource resource;
	};

	struct CResourcesFactory::DependencyPackage {
		DependencyPackageStatus status;
		u32 dependenciesCount;
		char* buffer;
		ResourceDependencyEvent* dependencies;
		u64 bufferSize;
		h64 dependant;
		u64 typeID;
	};

	// @TODO MEMORY MANAGMENT

	//---------------------------------------------------------------------------
	CResourcesFactory::CResourcesFactory() :
		_allocator(nullptr),
		_tagAllocator(nullptr)
	{
		_loadEvents = {0};
		_readyEvents = {0};
	}

	//---------------------------------------------------------------------------
	CResourcesFactory::~CResourcesFactory() {
	}


	//---------------------------------------------------------------------------
	void CResourcesFactory::Init(core::IAllocator* allocator, core::ITagAllocator* tagAllocator) {
		_allocator = allocator;
		_tagAllocator = tagAllocator;

		_packageMap.Init(allocator);
		_dependencyMap.Init(allocator);
		_constructorMap.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::RegisterTypeConstructor(u64 typeID, IResourceConstructor* constructor) {
		ASSERT(!_constructorMap.Find(typeID));

		_constructorMap.Add(typeID, constructor);
	}

	//---------------------------------------------------------------------------
	IResourceConstructor* CResourcesFactory::TypeIDToResourceConstructor(u64 typeID) {
		auto it = _constructorMap.Find(typeID);
		ASSERT(it);
		return *it;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::Update(const Frame* frame) {
		void* data = nullptr;

		ASSERT(!_loadEvents.count);

		u32 deleteCount = 0;
		// Update dependency packages (O(k*N) (dependency count))
		for (auto& package : _packageMap) {
			switch (package.status) {
			case DependencyPackageStatus::NEW: UpdateNewPackage(&package, frame); break;
			case DependencyPackageStatus::PENDING: UpdatePendingPackage(&package); break;
			case DependencyPackageStatus::READY: UpdateReadyPackage(&package, frame); break;
			case DependencyPackageStatus::EVENTS_SENT: UpdateEventsSentPackage(&package, frame); break;
			case DependencyPackageStatus::DELETE: ++deleteCount;  break; // separate cycle
			default:
				ASSERT(false);
				break;
			}
		}

		if (deleteCount > 0) {
			// Delete finished dependency packages
			auto it = _packageMap.CIterator();
			u32 count = it.count;
			u32 i = 0;
			while (i < count) {
				if (it.values[i].status == DependencyPackageStatus::DELETE) {
					_allocator->Deallocate(it.values[i].dependencies);
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

	//---------------------------------------------------------------------------
	void CResourcesFactory::GetLoadEvents(ResourceDependencyLoadEvents* evt) {
		evt->count = _loadEvents.count;
		evt->typesID = _loadEvents.typesID;
		evt->hashes = _loadEvents.hashes;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::GetReadyEvents(ResourceReadyEvents* evt) {
		evt->count = _readyEvents.count;
		evt->hashes = _readyEvents.hashes;
		evt->resources = _readyEvents.resources;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::ClearPendingEvents() {
		_loadEvents.capacity = 0;
		_loadEvents.count = 0;
		_loadEvents.typesID = nullptr;
		_loadEvents.hashes = nullptr;

		_readyEvents.count = 0;
		_readyEvents.capacity = 0;
		_readyEvents.hashes = nullptr;
		_readyEvents.resources = nullptr;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::OnDependencyReady(const ResourceReadyEvents* evt) {
		const h64* hashes = evt->hashes;
		const Resource* resources = evt->resources;
		
		for (u32 i = 0; i < evt->count; ++i) {
			PendingDependency* it = _dependencyMap.Find(evt->hashes[i]);
			// IT DOESNT HAS TO EXIST
			if (it) {
				it->resource = evt->resources[i];
				it->status = PendingDependencyStatus::READY;
			}
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::OnNewResource(const ResourceDataLoadedEvents* evt) {
		for (u32 i = 0; i < evt->count; ++i) {
			IResourceConstructor* constructor = TypeIDToResourceConstructor(evt->typeIDs[i]);
			u32 dependenciesCount = constructor->DependenciesCount(evt->buffers[i]);

			DependencyPackage package;
			package.status = DependencyPackageStatus::NEW;
			package.dependenciesCount = dependenciesCount;
			package.typeID = evt->typeIDs[i];
			package.dependant = evt->hashes[i];
			package.dependencies = nullptr;
			package.buffer = evt->buffers[i]; // ERR
			package.bufferSize = evt->bufferSizes[i];

			if (dependenciesCount)
				package.dependencies = static_cast<ResourceDependencyEvent*>(_allocator->Allocate(dependenciesCount * sizeof(ResourceDependencyEvent), alignof(ResourceDependencyEvent)));

			_packageMap.Add(evt->hashes[i], package);

			constructor->FillDependencies(evt->buffers[i], package.dependencies);
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::UpdateNewPackage(DependencyPackage* package, const Frame* frame) {
		ASSERT(package->status == DependencyPackageStatus::NEW);

		// not nice checking always this
		// ensure all, even though some dependencies can already in pending state
		if (_loadEvents.count + package->dependenciesCount > _loadEvents.capacity) {
			ReallocateLoadEventsBuffer(frame);
			ASSERT(_loadEvents.count + package->dependenciesCount <= _loadEvents.capacity);
		}

		// The ready count is in pending package too, but this will shorten the load by one frame, if its already ready
		u32 readyCount = 0;

		for (u32 i = 0; i < package->dependenciesCount; ++i) {
			ResourceDependencyEvent& dep = package->dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dep.hash);

			if (it) {
				if (it->status == PendingDependencyStatus::READY) {
					dep.resource = it->resource;
					++readyCount;
				}
				++it->dependantsCount; // Its dependant even if its ready
			} 
			else {
				PendingDependency newDep;
				newDep.dependantsCount = 1;
				newDep.resource = 0;
				newDep.status = PendingDependencyStatus::PENDING;
				_dependencyMap.Add(dep.hash, newDep);

				_loadEvents.typesID[_loadEvents.count] = dep.typeID;
				_loadEvents.hashes[_loadEvents.count] = dep.hash;
				++_loadEvents.count;
			}
		}

		package->status = readyCount == package->dependenciesCount ? DependencyPackageStatus::READY : DependencyPackageStatus::PENDING;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::UpdatePendingPackage(DependencyPackage* package) {
		ASSERT(package->status == DependencyPackageStatus::PENDING);

		u32 readyCount = 0;

		for (u32 i = 0; i < package->dependenciesCount; ++i) {
			ResourceDependencyEvent& dep = package->dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dep.hash);
			ASSERT(it); // has to exists
			if (it->status == PendingDependencyStatus::READY)
				++readyCount;
			dep.resource = it->resource;
		}

		if (readyCount == package->dependenciesCount)
			package->status = DependencyPackageStatus::READY;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::UpdateReadyPackage(DependencyPackage* package, const Frame* frame) {
		ASSERT(package->status == DependencyPackageStatus::READY);
		if (_readyEvents.count >= _readyEvents.capacity) {
			ReallocateFinishedEventsBuffer(frame);
			ASSERT(_readyEvents.count + package->dependenciesCount <= _readyEvents.capacity);
		}

		DependencyParams params = { package->dependant, package->dependenciesCount, package->dependencies };
		IResourceConstructor* factory = TypeIDToResourceConstructor(package->typeID);;

		Resource resource;
		factory->Create(package->buffer, &params, resource);

		_readyEvents.hashes[_readyEvents.count] = package->dependant;
		_readyEvents.resources[_readyEvents.count] = resource;

		++_readyEvents.count;
		package->status = DependencyPackageStatus::EVENTS_SENT;
	}


	//---------------------------------------------------------------------------
	void CResourcesFactory::UpdateEventsSentPackage(DependencyPackage* package, const Frame* frame) {
		ASSERT(package->status == DependencyPackageStatus::EVENTS_SENT);

		for (u32 i = 0; i < package->dependenciesCount; ++i) {
			ResourceDependencyEvent& dep = package->dependencies[i];
			PendingDependency* it = _dependencyMap.Find(dep.hash);
			ASSERT(it); // has to exists
			--it->dependantsCount;
		}

		package->status = DependencyPackageStatus::DELETE;
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::ReallocateLoadEventsBuffer(const Frame* frame) {
		h64* hashes;
		u64* typesID;

		const u64 ELEMENTS_SIZE = (sizeof(h64) + sizeof(u64));
		const u64 EXTRA_SIZE = alignof(u64);
		u64 size = (_loadEvents.capacity ? 2 * _loadEvents.capacity : 128) * ELEMENTS_SIZE + EXTRA_SIZE;

		u64 allocated;
		hashes = static_cast<h64*>(_tagAllocator->AllocateWithTag(frame->gameTag, size, alignof(h64), &allocated));
		u32 capacity = static_cast<u32>((allocated - EXTRA_SIZE) / ELEMENTS_SIZE);

		typesID = static_cast<u64*>(core::mem::Align(&hashes[capacity], alignof(u64)));

		LoadEvents oldLoadEvents = _loadEvents;

		_loadEvents.capacity = capacity;
		_loadEvents.count = oldLoadEvents.count;
		_loadEvents.typesID = typesID;
		_loadEvents.hashes = hashes;

		if (oldLoadEvents.typesID) {
			Memcpy(_loadEvents.hashes, oldLoadEvents.hashes, oldLoadEvents.count * sizeof(h64));
			Memcpy(_loadEvents.typesID, oldLoadEvents.typesID, oldLoadEvents.count * sizeof(u64));
		}
	}

	//---------------------------------------------------------------------------
	void CResourcesFactory::ReallocateFinishedEventsBuffer(const Frame* frame) {
		h64* hashes;
		Resource* resources;

		const u64 ELEMENTS_SIZE = (sizeof(h64) + sizeof(Resource));
		const u64 EXTRA_SIZE = alignof(Resource);
		u64 size = (_readyEvents.capacity ? 2 * _readyEvents.capacity : 128) * ELEMENTS_SIZE + EXTRA_SIZE;

		u64 allocated;
		hashes = static_cast<h64*>(_tagAllocator->AllocateWithTag(frame->gameTag, size, alignof(h64), &allocated));
		u32 capacity = static_cast<u32>((allocated - EXTRA_SIZE) / ELEMENTS_SIZE);

		resources = static_cast<Resource*>(core::mem::Align(&hashes[capacity], alignof(Resource)));

		ReadyEvents oldFinishedEvents = _readyEvents;

		_readyEvents.count = oldFinishedEvents.count;
		_readyEvents.capacity = capacity;
		_readyEvents.hashes = hashes;
		_readyEvents.resources = resources;

		if (oldFinishedEvents.count) {
			Memcpy(_readyEvents.hashes, _readyEvents.hashes, _readyEvents.count * sizeof(h64));
			Memcpy(_readyEvents.resources, _readyEvents.resources, _readyEvents.count * sizeof(Resource));
		}
	}

}