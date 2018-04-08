#include "CWorldModule.h"

#include <Core/Common/Assert.h>

#include "Engine/Engine.h"

#include "Engine/Modules/CStaticConstructor.h"

#include "Engine/Memory/CMemoryModule.h"

#include "Engine/World/Manager/CEntityManager.h"
#include "Engine/World/Manager/CTransform2DComponentManager.h"

namespace eng {
	//---------------------------------------------------------------------------
	CWorldModule::CWorldModule() :
		_state(ModuleState::CREATED) {
	}

	//---------------------------------------------------------------------------
	CWorldModule::~CWorldModule() {
		_transform2DManager->~CTransform2DComponentManager();
		_entityManager->~CEntityManager();
	}

	//---------------------------------------------------------------------------
	ModuleState CWorldModule::State() {
		return _state;
	}

	//---------------------------------------------------------------------------
	void CWorldModule::ConstructSubsytems(CStaticConstructor* constructor) {
		ASSERT(_state == ModuleState::CREATED);
		_state = ModuleState::UNINITIALIZED;

		_entityManager = constructor->Construct<CEntityManager>();
		_transform2DManager = constructor->Construct<CTransform2DComponentManager>();
	}

	//---------------------------------------------------------------------------
	void CWorldModule::Init(Engine* engine) {
		ASSERT(_state == ModuleState::UNINITIALIZED);
		_state = ModuleState::OK;

		core::IAllocator* all = engine->memoryModule->Allocator();
	}

	//---------------------------------------------------------------------------
	void CWorldModule::Update() {

	}



}