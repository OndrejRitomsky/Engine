#pragma once

#include <Core/Common/Types.h>

#include "Engine/Modules/ModuleState.h"

namespace render {
	class IRenderDevice;
}

namespace eng {
	struct Engine;

	class CStaticConstructor;
	class CEntityManager;
	class CTransform2DComponentManager;

	class CWorldModule {
	public:
		CWorldModule();
		~CWorldModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
		void Init(Engine* engine);

		void Update();

	private:
		CEntityManager* _entityManager;
		CTransform2DComponentManager* _transform2DManager;

		ModuleState _state;
	};

}