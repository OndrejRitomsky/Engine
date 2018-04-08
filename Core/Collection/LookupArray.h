#pragma once

#include "Core/Common/Types.h"
#include "Core/Common/Handle.h"

#include "Core/Collection/Base/InPlaceLinkedList.h"


namespace core {

	class IAllocator;

	// Doesnt support reserve, only as initial capacity
	// Doesnt support clear, doesnt know what is hole (external keys must be held)
	// !! Upon destruction array has to empty, implementation does not know where
	//    holes are and therefore cant call destructors
	// Sizeof Type has to be >= 8

	template<typename Type>
	class LookupArray {
	private:
		static const u32 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

	public:
		LookupArray();
		~LookupArray();

		LookupArray(const LookupArray& oth) = delete;
		LookupArray& operator=(const LookupArray& rhs) = delete;

		void Init(IAllocator* allocator, u32 capacity = 0);
		void Remove(Handle handle);

		const Type& Get(Handle handle) const;
		Type& Get(Handle handle);

		Handle Add(const Type& value);
		Handle Add(Type&& value);

	private:
		void ReserveCapacity();

		u32 Conversion(Handle handle) const;

	private:
		u32 _count;
		u32 _capacity;
		InPlaceLinkedList _linked;
		IAllocator* _allocator;
	};
}