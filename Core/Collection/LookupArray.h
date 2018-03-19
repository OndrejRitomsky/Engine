#pragma once

#include "Core/Types.h"

#include "Core/Collection/Base/LookupArrayBase.h"


namespace core {

	class IAllocator;

	// Doesnt support reserve, only as initial capacity
	// Doesnt support clear, doesnt know what is hole (external keys must be held)
	// !! Upon destruction array has to empty, implementation does not know where
	//    holes are and therefore cant call destructors
	template<typename Type>
	class LookupArray {
	private:
		static const u32 UNINITIALIZED_PUSH_INIT_CAPACITY = 8;

	public:
		LookupArray();
		~LookupArray();

		LookupArray(const LookupArray& oth) = delete;
		LookupArray& operator=(const LookupArray& rhs) = delete;

		void Init(IAllocator* allocator, u32 capacity);
		void Remove(u32 index);

		const Type& Get(u32 index) const;
		Type& Get(u32 index);

		u32 Add(const Type& value);
		u32 Add(Type&& value);

	private:
		void ReserveCapacity();

	private:
		LookupArrayBase _base;
		IAllocator* _allocator;
	};
}