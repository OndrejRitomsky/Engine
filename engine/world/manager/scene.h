#pragma once

#include "../../util/handle_manager.h"

#include "../entity.h"

namespace core {
	struct IAllocator;
}

namespace eng
{
	struct Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Init(core::IAllocator* allocator);

		// Entity will be initialized and registered
		Entity AddEntity();

		// Entity will deinitialized and unregistered
		void RemoveEntity(Entity* e);

		// @TODO
	private:
		HandleManager _entities;
	};
}

