#pragma once


#include <core/common/types.h>
#include <core/collection/buffer.h>

namespace eng {

	//class Buffer;

	/*class ReadStream {
	public:
		enum { IsReading = 1 };
		enum { IsWriting = 0 };

	public:
		ReadStream();
		~ReadStream();

		void Init(core::Buffer* buffer);
		bool SerializeU32(u32& value);

	private:
		u32 _offset;
		
		core::Buffer* _buffer;
	};

	ReadStream::ReadStream() :
		_offset(0),
		_buffer(nullptr) {
	}
	ReadStream::~ReadStream() {}

	void ReadStream::Init(core::Buffer* buffer) {
		_buffer = buffer;
	}

	bool ReadStream::SerializeU32(u32& value) {
		if (_buffer->Size() + _offset + sizeof(u32) <= _buffer->Size()) {
			u32 v;
			_buffer->Read((char*) (&v), _offset, sizeof(u32));
			value = v;  // @TODO ENDIANESS
			_offset += sizeof(u32);
			return true;
		}

		return false;
	}*/
}