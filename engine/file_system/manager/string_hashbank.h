#pragma once


#include <core/common/types.h>
#include <core/collection/hashmap.h>



namespace core {
	struct IAllocator;
}

namespace eng {
	class StringHashBank {
	public:
		StringHashBank();
		~StringHashBank();

		void Init(core::IAllocator* allocator, core::IAllocator* stringAllocator);

		void Add(h64 hash, const char* text);

		// Size with NULL terminator!
		void Add(h64 hash, const char* text, u32 size);

		const char* Get(h64 hash) const;

	private:
		core::HashMap<char*> _texts;
		core::IAllocator*    _stringAllocator;
	};
}
