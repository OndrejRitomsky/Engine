#pragma once

#include "Core/Common/Types.h"

namespace core {

	class IAllocator;


	//@TODO REMOVE (plain arrays) OR IMPROVE
	class Buffer {
	public:
		Buffer(IAllocator* allocator);
		~Buffer();

		void Init(u32 length);

		void Append(char* data, u32 length);
		void Read(char* data, u32 offset, u32 length);

		char* Data();
		u32 Size();
		u32 Capacity();

	private:
		u32   _size;
		u32   _capacity;
		
		char*      _data;
		IAllocator* _allocator;
	};
}