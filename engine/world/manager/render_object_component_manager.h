#pragma once

#include <core/collection/hashmap.h>


namespace core {
	struct IAllocator;
}

namespace eng {

	struct Entity;

	struct RenderObjectComponent {
		core::Handle h;
	};

	class RenderObjectComponentManager {
	public:
		RenderObjectComponentManager();
		~RenderObjectComponentManager();

		void Init(core::IAllocator* allocator);
		void Reserve(u32 capacity);

		RenderObjectComponent Find(const Entity* entity) const;

		RenderObjectComponent Create(Entity* entity);
		bool Remove(Entity* entity);

		h64 ResourceHash(RenderObjectComponent handle) const;
		void SetResourceHash(RenderObjectComponent handle, h64 hash);

		void GetEntity(RenderObjectComponent handle, Entity* outEntity) const;

		const RenderObjectComponent* RenderComponents(u32* outCount) const;

	private:
		void Reallocate(u32 capacity);
		void SetEmpty(RenderObjectComponent handle);

	private:
		struct Data {
			u32 count;
			u32 capacity;
			void* data;

			Entity* ids;
			h64*    resourceHash;
		};

		Data _data;
		core::HashMap<RenderObjectComponent> _map;
		core::IAllocator*  _allocator;
	};
}

