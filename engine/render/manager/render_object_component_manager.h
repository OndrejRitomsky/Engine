#pragma once

#include <core/collection/hashmap.h>
#include "../../resources/resource.h"
#include "../../world/entity.h"

namespace core {
	struct IAllocator;
}

namespace eng {

	struct RenderObjectComponent {
		core::Handle h;
	};

	class RenderObjectComponentManager {
	public:
		RenderObjectComponentManager();
		~RenderObjectComponentManager();

		void Init(core::IAllocator* allocator);
		void Reserve(u32 capacity);

		bool Find(Entity entity, RenderObjectComponent* outComponent) const;

		RenderObjectComponent Create(Entity entity);
		bool Remove(Entity entity);

		bool GetReady(RenderObjectComponent handle) const;
		void SetReady(RenderObjectComponent handle, bool ready);

		Resource GetResource(RenderObjectComponent handle) const;
		void SetResource(RenderObjectComponent handle, Resource resource);

		h64 GetResourceHash(RenderObjectComponent handle) const;
		void SetResourceHash(RenderObjectComponent handle, h64 hash);

		Entity GetEntity(RenderObjectComponent handle) const;

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
			Resource* resources;
			h64* resourcesHash;
			bool* resourcesReady;
		};

		Data _data;
		core::HashMap<RenderObjectComponent> _map;
		core::IAllocator*  _allocator;
	};
}

