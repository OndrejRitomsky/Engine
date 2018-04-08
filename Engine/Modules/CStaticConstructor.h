#pragma once

#include <Core/Common/Types.h>
#include <Core/Common/Placement.h>

namespace eng {
	class CStaticConstructor {
	public:
		CStaticConstructor();
		~CStaticConstructor() = default;

		void Init(void* buffer, u64 size);
		void* GetBuffer();

		template<typename Type>
		Type* Construct();

	private:
		void* Next(u32 size, u32 alignment);

	private:
		void* _bufferStart;
		void* _bufferCurrent;
		u64 _size;
	};


	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	template<typename Type>
	Type* CStaticConstructor::Construct() {
		return Placement(Next(sizeof(Type), alignof(Type))) Type();

		// @TODO this should be aligned to cache line
	}
}