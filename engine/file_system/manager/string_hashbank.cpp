#include "string_hashbank.h"

#include <string.h>
#include <core/collection/hashmap.inl>
#include <core/allocator/allocate.h>

namespace eng {

	StringHashBank::StringHashBank() :
		_stringAllocator(nullptr) {
	}

	StringHashBank::~StringHashBank() {
		for (auto& value : _texts)
			Deallocate(_stringAllocator, value);
	}

	void StringHashBank::Init(core::IAllocator* allocator, core::IAllocator* stringAllocator) {
		_stringAllocator = stringAllocator;
		_texts.Init(allocator);
	}

	void StringHashBank::Add(h64 hash, const char* text, u32 length) {
		void* data = Allocate(_stringAllocator, length, alignof(char));
		core::Memcpy(data, text, length);
		bool res = _texts.Add(hash, static_cast<char*>(data));

		if (!res)
			Deallocate(_stringAllocator, data);
	}

	void StringHashBank::Add(h64 hash, const char* text) {
		u32 len = (u32) strlen(text) + 1;
		Add(hash, text, len);
	}

	const char* StringHashBank::Get(h64 hash) const {
		const char* const* result = _texts.Find(hash);
		if (!result)
			return nullptr;

		return *result;
	}
}
