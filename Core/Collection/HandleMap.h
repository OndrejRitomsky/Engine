#pragma once

#include "Core/Common/Types.h"
#include "Core/Common/Handle.h"

#include "Core/Collection/Base/HandleMapBase.h"

namespace core {

	class IAllocator;

	/**
	* HandleMap<Type> : O(1) lookup container
	* Double indirection for lookup
	* Data are contiguous, iterating is without indirection
	*/

	// @TODO @REWORK 3 arrays into SOA
	template<typename Type>
	class HandleMap {
	private:
		static const u32 PUSH_INIT_CAPACITY = 8;

	public:
		HandleMap();
		~HandleMap();

		HandleMap(const HandleMap& oth) = delete;
		HandleMap& operator=(const HandleMap& oth) = delete;

		void Init(IAllocator* allocator, u16 handleType = 0);

		// Handle has to be valid!
		inline Type& Get(const Handle& handle);

		// Handle has to be valid!
		inline const Type& Get(const Handle& handle) const;

		inline void Reserve(u32 capacity);

		inline bool IsValid(const Handle& handle) const;

		inline u32 Count() const;
		inline u32 Capacity() const;

		inline Type* begin();
		inline Type* end();

		inline const Type* begin() const;
		inline const Type* end() const;

		Handle Add(const Type& value);

		void Remove(const Handle& handle);

	private:
		HandleMapBase _base;
		IAllocator* _allocator;
		Type* _values;
	};
}