#pragma once

#include <Core/Common/Types.h>
#include <Core/Collection/HashMap.h>
#include <Core/Collection/LookupArray.h>

namespace eng {
	// We need mapping render object into render::resources (should render object be realy resource, maybe move to renderer)
	// but we can release render object when everything is inside gpu
	// but we cant release everything maybe only textures (we cant things that change)
	// or maybe we should create copies if the things change
	// or maybe that will beo nly events


	class CRenderResourceManager {
	private:
		struct RenderObjectInfo;

	public:
		CRenderResourceManager();
		~CRenderResourceManager();

	private:
		//h64Map<RenderObjectInfo>
	};

}

