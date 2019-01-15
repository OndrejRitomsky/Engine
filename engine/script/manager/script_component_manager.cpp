#include "script_component_manager.h"

#include <core/allocator/allocate.h>
#include <core/algorithm/cstring.h>
#include <core/common/pointer.h>
#include <core/common/utility.h>
#include <core/collection/hashmap.inl>
#include <core/collection/array.inl>

#include "../iscript.h"

#include "../../world/entity.h"

#include "script_types.h"

#include "../../engine.h"


const u64 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

namespace eng {
	ScriptComponentManager::ScriptComponentManager() {}

	ScriptComponentManager::~ScriptComponentManager() {
		for (auto typeComponents : _components) {
			for (u32 j = 0; j < typeComponents.count; ++j) {
				typeComponents.script.Deinit(typeComponents.objects[j]);
				core::Deallocate(_allocator, typeComponents.objects[j]);
			}

			if (typeComponents.count > 0) {
				Deallocate(_allocator, typeComponents.data);
			}
		}
	}

	void ScriptComponentManager::Init(Engine* engine, core::IAllocator* allocator) {
		ASSERT(!_allocator);

		_allocator = allocator;
		//_map.Init(allocator);
		_names.Init(allocator);
		_components.Init(allocator);

		_engine = engine;
	}

	void ScriptComponentManager::Update() {
		for (auto typeComponents : _components) {
			auto UpdateFunc = typeComponents.script.Update;

			for (u32 j = 0; j < typeComponents.count; ++j) {
				UpdateFunc(typeComponents.objects[j]);
			}
		}
	}




	void ScriptComponentManager::Reserve(u32 capacity) {
		//if (capacity > _data.capacity)
		//	Reallocate(capacity);
	}

	void ScriptComponentManager::SetEmpty(ScriptComponent handle) {
		u64 i = handle.h;
		
	}

	void ScriptComponentManager::Create(Entity entity, const char* scriptName) {
		ScriptComponent handle;
		handle.h = 0;

		ScriptTypeComponents* typeComponents;

		u32 i = 0;
		for (; i < _names.Count(); ++i) {
			if (core::StrEqual(_names[i], scriptName)) {
				typeComponents = &_components[i];
				break;
			}
		}

		if (i == _names.Count()) {
			ScriptTypeComponents newComponent;
			newComponent.count = 0;
			newComponent.capacity = 0;
			newComponent.data = nullptr;
			newComponent.id = nullptr;
			newComponent.objects = nullptr;

			if (!ScriptTypeFind(scriptName, &newComponent.info, &newComponent.script))
				return; // false

			// register new type
			_names.Push(newComponent.info.name);
			_components.Push(newComponent);

			typeComponents = &_components[_components.Count() - 1];
		}


		AddTypeComponent(typeComponents, entity);
	}

	void ScriptComponentManager::AddTypeComponent(ScriptTypeComponents* typeComponents, Entity entity) {
		if (typeComponents->count == typeComponents->capacity)
			ReallocateTypeComponent(typeComponents, core::Max<u32>((typeComponents->capacity * 2), UNINITIALIZED_PUSH_INIT_CAPACITY));

		u32 index = typeComponents->count;
		typeComponents->count++;

		typeComponents->id[index] = entity;
		typeComponents->objects[index] = core::Allocate(_allocator, typeComponents->info.size, typeComponents->info.alignment);

		typeComponents->script.Init(typeComponents->objects[index], _engine, entity);
	}


	/*bool ScriptComponentManager::Remove(Entity entity) {
		Transform2DComponent* handle = _map.Find(entity.Hash());
		if (!handle)
			return false;

		u32 last = _data.count - 1;
		u64 i = static_cast<u64>(handle->h);

		if (i < last) {
			Transform2DComponent* lastHandle = _map.Find(_data.id[last].Hash());
			ASSERT(lastHandle);

			_data.id[i] = _data.id[last];
			_data.dirty[i] = _data.dirty[last];
			_data.angle[i] = _data.angle[last];
			_data.position[i] = _data.position[last];
			_data.scale[i] = _data.scale[last];
			_data.transform[i] = _data.transform[last];

			lastHandle->h = i;
		}

		--_data.count;
		return true;
	}*/

	void ScriptComponentManager::ReallocateTypeComponent(ScriptTypeComponents* typeComponents, u32 capacity) {
		ASSERT(capacity > 0);

		u32 sizeNeeded = capacity * (sizeof(Entity) + sizeof(void*));
		sizeNeeded += alignof(Entity) + alignof(void*);

		ScriptTypeComponents data;
		data.script = typeComponents->script;
		data.info = typeComponents->info;
		data.count = typeComponents->count;
		data.capacity = capacity;		
		data.data = Allocate(_allocator, sizeNeeded, 1);

		{
			data.id = (Entity*) core::PointerAlign(data.data, alignof(Entity));
			data.objects = (void**) core::PointerAlign(&data.id[data.capacity], alignof(void*));
		}

		if (typeComponents->count > 0) {
			core::Memcpy(data.id, typeComponents->id, data.count * sizeof(Entity));
			core::Memcpy(data.objects, typeComponents->objects, data.count * sizeof(void*));

			Deallocate(_allocator, typeComponents->data);
		}

		*typeComponents = data;
	}
}
