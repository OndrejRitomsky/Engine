#pragma once

#include "../../util/handle_manager.h"

namespace core {
	struct IAllocator;
}

namespace eng
{
	struct Entity;

	// @TODO this doesnt haveto exist handle manager can be in scene.. or this is global for multiple scenes?

	/*class CEntityManager
	{
	public:
		CEntityManager();
		~CEntityManager();

		void Init(core::IAllocator* allocator);

		void InitEntity(Entity* entity);

		// @TODO
	private:
		HandleManager _entities;
	};*/
}

