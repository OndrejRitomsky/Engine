#pragma once

#include <core/common/types.h>
#include "resource.h"

namespace eng {
	Resource HandleToResource(core::Handle handle);

	core::Handle ResourceToHandle(Resource resource);


	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------



	
	//---------------------------------------------------------------------------
	inline Resource HandleToResource(core::Handle handle) {
		ASSERT(sizeof(core::Handle) == sizeof(Resource));
		return reinterpret_cast<Resource>(handle);
	}

	//---------------------------------------------------------------------------
	inline core::Handle ResourceToHandle(Resource resource) {
		ASSERT(sizeof(core::Handle) == sizeof(Resource));
		return reinterpret_cast<core::Handle>(resource);
	}
}