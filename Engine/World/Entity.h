#pragma once

#include <Core/Common/Handle.h>

namespace eng {

	// Just strong typedef
	struct Entity {
		bool operator==(Entity rhs);
		bool operator!=(Entity rhs);

		// Valid internaly does not have to mean valid as entity (for that is entity manager)
		// this maybe should be moved to the entity manager
		bool IsValid();

		static Entity MakeInvalid();

		core::Handle h;
	};

}