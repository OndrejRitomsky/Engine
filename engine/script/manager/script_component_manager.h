#pragma once

#include <core/common/types.h>
#include <core/collection/hashmap.h>
#include <core/collection/array.h>

#include "../../world/entity.h"

#include "../iscript.h" // hm

namespace core {
	struct IAllocator;
}

namespace eng {

	struct Engine;

	struct ScriptComponent {
		u32 scriptID;
		core::Handle h;
	};

	class ScriptComponentManager {
	public:
		ScriptComponentManager();
		~ScriptComponentManager();

		void Init(Engine* engine, core::IAllocator* allocator);
		void Reserve(u32 capacity);

		void Create(Entity entity, const char* scriptName);
		//bool Remove(Entity entity);

		void Update();

	private:
		struct ScriptTypeComponents;
		void AddTypeComponent(ScriptTypeComponents* typeComponents, Entity entity);
		void ReallocateTypeComponent(ScriptTypeComponents* typeComponetns, u32 capacity);

		void SetEmpty(ScriptComponent handle);

		

	private:
		// data for only one type of script
		struct ScriptTypeComponents {
			IScript script;
			ScriptInfo info;

			u32 count;
			u32 capacity;
			void* data;
			Entity* id;
			void** objects; // must be array of pointers, data cant be moved !!! 
		};

		Engine* _engine;

		//core::HashMap<ScriptComponent> _map;
		core::Array<const char*> _names; // Duplicit by type _types can be rather big
		core::Array<ScriptTypeComponents> _components;

		// @TOOD this has to be move effective then just one by one allocating!
		// (64,128,256,512, < );
		core::IAllocator* _allocator = nullptr;
	};
}

