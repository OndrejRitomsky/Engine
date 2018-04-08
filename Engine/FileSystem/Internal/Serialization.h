#pragma once

#include <Core/Common/Types.h>

namespace eng {

	template<typename Stream>
	bool SerializeU32(Stream& stream, u32& value);


	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------

	

	//----------------------------------------------------------------------------
	template<typename Stream>
	bool SerializeU32(Stream& stream, u32& value) {
		u32 val;
		if (Stream::IsWriting)
			val = value;

		if (!stream.SerializeU32(val))
			return false;

		if (Stream::IsReading)
			value = val;

		return true;
	}
}

