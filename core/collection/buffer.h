#pragma once

#include "../common/types.h"

namespace core {
	struct IAllocator;

	class Buffer {
	public:
		Buffer();
		~Buffer();

		void Init(IAllocator* allocator);
		void Reserve(u64 size);
		void Append(char* data, u64 size);
		void AppendUnchecked(char* data, u64 size);
		void Clear();


		char* Data();

		u64 Size();

	private:
		u64 _size;
		u64 _capacity;

		char* _data;
		IAllocator* _allocator;
	};
}
