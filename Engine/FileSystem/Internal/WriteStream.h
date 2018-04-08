#pragma once


#include <Core/Common/Types.h>
#include <Core/Collection/Buffer.h>

namespace eng {

	class WriteStream {
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
		if (_buffer->Size() + sizeof(u32) <= _buffer->Capacity()) {
			u32 v = value; // @TODO ENDIANESS
			_buffer->Append((char*) (&v), sizeof(u32));

			return true;
		}

		return false;
	}
}