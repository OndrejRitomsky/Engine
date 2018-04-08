#include "CStringHashBank.h"

#include <string.h>
#include <Core/Collection/HashMap.inl>
#include <Core/Allocator/IAllocator.inl>

namespace eng {

	//---------------------------------------------------------------------------
	CStringHashBank::CStringHashBank() :
		_stringAllocator(nullptr) {
	}

	//---------------------------------------------------------------------------
	CStringHashBank::~CStringHashBank() {
		for (auto& value : _texts)
			_stringAllocator->Deallocate(value);
	}

	//---------------------------------------------------------------------------
	void CStringHashBank::Init(core::IAllocator* allocator, core::IAllocator* stringAllocator) {
		_stringAllocator = stringAllocator;
		_texts.Init(allocator);
	}

	//---------------------------------------------------------------------------
	void CStringHashBank::Add(h64 hash, const char* text, u32 length) {
		void* data = _stringAllocator->Allocate(length, alignof(char));
		Memcpy(data, text, length);
		bool res = _texts.Add(hash, static_cast<char*>(data));

		if (!res)
			_stringAllocator->Deallocate(data);
	}

	//---------------------------------------------------------------------------
	void CStringHashBank::Add(h64 hash, const char* text) {
		u32 len = (u32) strlen(text) + 1;
		Add(hash, text, len);
	}

	//---------------------------------------------------------------------------
	const char* CStringHashBank::Get(h64 hash) const {
		const char* const* result = _texts.Find(hash);
		if (!result)
			return nullptr;

		return *result;
	}
}
