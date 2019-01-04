#pragma once

#include "../common/types.h"

#include "parallel_types.h"



// Is WriteRel, ReadAcq / Write, Read needed_

namespace core {

	void AtomicI32WriteRel(AtomicI32* atomic, i32 value);

	void AtomicI32Write(AtomicI32* atomic, i32 value);

	i32 AtomicI32ReadAcq(const AtomicI32* atomic);

	i32 AtomicI32Read(const AtomicI32* atomic);


	i32 AtomicI32Add(AtomicI32* atomic, i32 value);

	i32 AtomicI32AddRel(AtomicI32* atomic, i32 value);

	i32 AtomicI32Sub(AtomicI32* atomic, i32 value);

	i32 AtomicI32SubRel(AtomicI32* atomic, i32 value);


	bool AtomicBoolReadAcq(const AtomicBool* atomic);

	bool AtomicBoolRead(const AtomicBool* atomic);

	void AtomicBoolWriteRel(AtomicBool* atomic, bool value);

	void AtomicBoolWrite(AtomicBool* atomic, bool value);

	bool AtomicBoolExchange(AtomicBool* atomic, bool value);

	bool AtomicBoolExchangeAcq(AtomicBool* atomic, bool value);

}