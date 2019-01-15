#pragma once


#include <core/common/types.h>
#include <core/collection/buffer.h>

namespace eng {

	/*class WriteStream {
	public:
		enum { IsReading = 0 };
		enum { IsWriting = 1 };

	public:
		WriteStream();
		~WriteStream();

		void Init(core::Buffer* buffer);
		bool SerializeU32(u32 value);

	private:
		core::Buffer* _buffer;
	};

	WriteStream::WriteStream() :
		_buffer(nullptr) {
	}

	WriteStream::~WriteStream() {}

	void WriteStream::Init(core::Buffer* buffer) {
		_buffer = buffer;
	}

	bool WriteStream::SerializeU32(u32 value) {
		if (_buffer->Size() + sizeof(u32) <= _buffer->Size()) {
			u32 v = value; // @TODO ENDIANESS
			_buffer->Append((char*) (&v), sizeof(u32));

			return true;
		}

		return false;
	}*/
}