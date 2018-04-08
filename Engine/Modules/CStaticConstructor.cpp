#include "CStaticConstructor.h"

#include <Core/Common/Assert.h>
#include <Core/Common/Pointer.h>

#include <cstdlib> // exit

namespace eng {
	//---------------------------------------------------------------------------
	CStaticConstructor::CStaticConstructor() :
		_bufferStart(nullptr),
		_bufferCurrent(nullptr),
		_size(0) {
	}

	//---------------------------------------------------------------------------
	void CStaticConstructor::Init(void* buffer, u64 size) {
		_bufferStart = buffer;
		_bufferCurrent = _bufferStart;
		_size = size;
	}

	//---------------------------------------------------------------------------
	void* CStaticConstructor::Next(u32 size, u32 alignment) {
		ASSERT(_bufferCurrent);
		void* pos = core::mem::Align(_bufferCurrent, alignment);
		_bufferCurrent = core::mem::Add(pos, size);

		if (reinterpret_cast<u64>(_bufferCurrent) > reinterpret_cast<u64>(core::mem::Add(_bufferStart, _size))) {
			ASSERT(false);
			exit(1);
		}

		return pos;
	}

	//---------------------------------------------------------------------------
	void* CStaticConstructor::GetBuffer() {
		return _bufferStart;
	}
}