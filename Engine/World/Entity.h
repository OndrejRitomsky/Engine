#pragma once

#include <core/common/types.h>

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



	inline bool Entity::operator==(Entity rhs) {
		return h == rhs.h;
	}

	inline bool Entity::operator!=(Entity rhs) {
		return h != rhs.h;
	}

	inline bool Entity::IsValid() {
		return h != MakeInvalid().h;
	}

	inline Entity Entity::MakeInvalid() {
		return Entity{0};
	}

}