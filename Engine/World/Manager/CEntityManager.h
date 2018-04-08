#pragma once

#include "Engine/World/Entity.h"
#include "Engine/Util/CHandleManager.h"

namespace core {
	class IAllocator;
}

namespace eng
{
	class CEntityManager
	{
	public:
		CEntityManager();
		~CEntityManager();

		void Init(core::IAllocator* allocator);

		Entity AddEntity();

		// @TODO

	private:
		CHandleManager _entities;
	};
}

