#include "CEntityManager.h"

namespace eng {

	//---------------------------------------------------------------------------
	CEntityManager::CEntityManager() {
	}
	
	//---------------------------------------------------------------------------
	CEntityManager::~CEntityManager() {
	}

	//---------------------------------------------------------------------------
	void CEntityManager::Init(core::IAllocator* allocator) {
		_entities.Init(allocator);
	}

	//---------------------------------------------------------------------------
	Entity CEntityManager::AddEntity() {
		return { _entities.NewHandle() };
	}
	
}