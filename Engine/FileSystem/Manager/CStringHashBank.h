#pragma once


#include <core/common/types.h>
#include <Core/Collection/HashMap.h>



namespace core {
	struct IAllocator;
}

namespace eng {
	class CStringHashBank {
	public:
		CStringHashBank();
		~CStringHashBank();

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
