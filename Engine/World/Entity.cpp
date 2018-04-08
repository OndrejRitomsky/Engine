#include "Entity.h"

namespace eng {

	//---------------------------------------------------------------------------
	bool Entity::operator==(Entity rhs) {
		return h == rhs.h;
	}

	//---------------------------------------------------------------------------
	bool Entity::operator!=(Entity rhs) {
		return h != rhs.h;
	}

	//---------------------------------------------------------------------------
	bool Entity::IsValid() {
		return h != MakeInvalid().h;
	}

	//---------------------------------------------------------------------------
	Entity Entity::MakeInvalid() {
		return Entity{0};
	}
	
}