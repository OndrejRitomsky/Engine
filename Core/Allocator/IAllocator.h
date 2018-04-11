#pragma once

#include "Core/Common/Types.h"

namespace core {
	class IAllocator {
	public:
		// Optionaly get real allocated size (outAllocated >= size)
		void* Allocate(u64 size, u64 alignment, u64* outAllocated = nullptr);

		void Deallocate(void* address);

		void Deinit();

		// :( templates, :( implemented functions in interface

		template<typename T, typename... Args>
		T* Make(Args&&... args);

		template<typename T>
		void Destroy(T* object);

		// this has to be protected, only valid way to set them is STATIC_CAST from derived class
	protected:
		typedef void*(IAllocator::*AllocateFunction)(u64 size, u64 alignment, u64* outAllocated);
		typedef void(IAllocator::*DeallocateFunction)(void* address);
		typedef void(IAllocator::*DeinitFunction)();

		AllocateFunction _Allocate;
		DeallocateFunction _Deallocate;
		DeinitFunction _Deinit;
	};
}


