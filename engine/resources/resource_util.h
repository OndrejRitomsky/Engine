#pragma once

#include <core/common/types.h>
#include <core/common/debug.h>

#include "resource.h"

namespace eng {
	Resource HandleToResource(core::Handle handle);

	core::Handle ResourceToHandle(Resource resource);




	inline Resource HandleToResource(core::Handle handle) {
		ASSERT(sizeof(core::Handle) == sizeof(Resource));
		return (Resource) handle;
	}

	inline core::Handle ResourceToHandle(Resource resource) {
		ASSERT(sizeof(core::Handle) == sizeof(Resource));
		return (core::Handle) resource;
	}
}