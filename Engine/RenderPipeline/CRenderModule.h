#pragma once

#include <Core/Common/Types.h>

#include "Engine/Modules/ModuleState.h"

namespace render {
	class IRenderDevice;
}
namespace core {
	class ITagAllocator;
}

namespace eng {
	struct Engine;

	class CStaticConstructor;

	class CRenderModule {
	public:
		CRenderModule();
		~CRenderModule();

		ModuleState State();
		void ConstructSubsytems(CStaticConstructor* constructor);
		void Init(Engine* engine);

		void Update();

		render::IRenderDevice* RenderDevice();

	private:
		ModuleState _state;

		u8 _rendererType;

		core::ITagAllocator* _taggedAllocator;
		render::IRenderDevice* _renderDevice;
	};

}