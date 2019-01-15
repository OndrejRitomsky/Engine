#pragma once

#include <core/common/types.h>

namespace eng {

	// CLEANUP ... functions should be separate... possible C style or .inl
	// Just strong typedef
	struct Entity {
		bool operator==(Entity rhs);
		bool operator!=(Entity rhs);

		// Valid internaly does not have to mean valid as entity (for that is entity manager)
		// this maybe should be moved to the entity manager
		bool IsValid();

		h64 Hash() const; // For easier use with hash maps

		static Entity FromHash(h64 hash); 

		static Entity MakeInvalid();

		core::Handle _handle;
	};



	inline bool Entity::operator==(Entity rhs) {
		return _handle == rhs._handle;
	}

	inline bool Entity::operator!=(Entity rhs) {
		return _handle != rhs._handle;
	}

	inline bool Entity::IsValid() {
		return _handle != MakeInvalid()._handle;
	}

	inline h64 Entity::Hash() const {
		return (h64) _handle;
	}

	inline Entity Entity::FromHash(h64 hash) {
		return {(core::Handle) hash};
	}

	inline Entity Entity::MakeInvalid() {
		return Entity{0};
	}
}