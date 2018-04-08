#include "InPlaceLinkedList.h"

#include "Core/Common/Assert.h"

#include "Core/Common/TypeLimits.h"

namespace core {

	static const u32 INVALID_INDEX = U32MAX;

	namespace in_place_linked_list {
		//-------------------------------------------------------------------------
		void Init(InPlaceLinkedList* linked, void* data) {
			linked->data = data;
			linked->freeFirst = INVALID_INDEX;
			linked->countWithHoles = 0;
		}

		//-------------------------------------------------------------------------
		void Clear(InPlaceLinkedList* linked) {
			linked->freeFirst = INVALID_INDEX;
			linked->countWithHoles = 0;
		}

		//-------------------------------------------------------------------------
		void Remove(InPlaceLinkedList* linked, u32 index, u32 elementSize) {
			ASSERT(index < linked->countWithHoles);
			ASSERT(elementSize >= sizeof(u32));

			u32* element = reinterpret_cast<u32*>(&static_cast<char*>(linked->data)[index * elementSize]);
			*element = linked->freeFirst;
			linked->freeFirst = index;
		}

		//-------------------------------------------------------------------------
		u32 Add(InPlaceLinkedList* linked, u32 elementSize) {
			ASSERT(elementSize >= sizeof(u32));

			if (linked->freeFirst == INVALID_INDEX) {
				++linked->countWithHoles;
				return linked->countWithHoles - 1;
			}
			else {
				u32 result = linked->freeFirst;
				u32* element = reinterpret_cast<u32*>(&static_cast<char*>(linked->data)[result * elementSize]);
				linked->freeFirst = *element;
				return result;
			}
		}
	}
}
