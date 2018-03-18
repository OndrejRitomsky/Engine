#pragma once

#include "Core/Types.h"

// @TODO REWORK NO INTERFACE

namespace core {
	class IAllocator {
	public:
		virtual ~IAllocator() = default;
		virtual void* Allocate(u64 size, u64 alignment) = 0;
		virtual void Deallocate(void* address) = 0;

		template<typename T, typename... Args>
		T* Make(Args&&... args);

		template<typename T>
		void Destroy(T* object);

		// maybe remove
		template<typename T, typename... Args>
		T* MakeArray(u32 count, Args&&... args);

		// maybe remove
		template<typename T>
		void DestroyArray(T* objects);
	};
}

