#include "atomic.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

namespace core {
	static_assert(sizeof(AtomicI32) == sizeof(long), "Size missmatch");
	static_assert(sizeof(AtomicBool) == sizeof(long), "Size missmatch");

	void AtomicI32WriteRel(AtomicI32* atomic, i32 value) {
		MemoryBarrier(); // all previous writes must happen before 
		*atomic = value;
	}

	void AtomicI32Write(AtomicI32* atomic, i32 value) {
		MemoryBarrier();
		*atomic = value;
		MemoryBarrier();
	}

	i32 AtomicI32ReadAcq(const AtomicI32* atomic) {
		i32 res = *atomic;
		MemoryBarrier(); // all subsequent reads must happen after
		return res;
	}

	i32 AtomicI32Read(const AtomicI32* atomic) {
		MemoryBarrier();
		i32 res = *atomic;
		MemoryBarrier();
		return res;
	}

	i32 AtomicI32Add(AtomicI32* atomic, i32 value) {
		return _InterlockedExchangeAdd((volatile long*) atomic, value);
	}

	i32 AtomicI32AddRel(AtomicI32* atomic, i32 value) {
		return _InterlockedExchangeAdd((volatile long*) atomic, value);
		//return _InterlockedExchangeAdd_rel((volatile long*) atomic, value);
	}

	i32 AtomicI32Sub(AtomicI32* atomic, i32 value) {
		return _InterlockedExchangeAdd((volatile long*) atomic, -value);
	}

	i32 AtomicI32SubRel(AtomicI32* atomic, i32 value) {
		return _InterlockedExchangeAdd((volatile long*) atomic, -value);
		//return  _InterlockedExchangeAdd_rel((volatile long*) atomic, -value);
	}

	 void AtomicBoolWriteRel(AtomicBool* atomic, bool value) {
		AtomicI32WriteRel(atomic, value);
	}

	void AtomicBoolWrite(AtomicBool* atomic, bool value) {
		AtomicI32Write(atomic, value);
	}

	bool AtomicBoolReadAcq(const AtomicBool* atomic) {
		return AtomicI32ReadAcq(atomic) != 0;
	}

	bool AtomicBoolRead(const AtomicBool* atomic) {
		return AtomicI32Read(atomic) != 0;
	}

	bool AtomicBoolExchange(AtomicBool* atomic, bool value) {
		return _InterlockedExchange((volatile long*) atomic, value) != 0;
	}

	bool AtomicBoolExchangeAcq(AtomicBool* atomic, bool value) {
		return _InterlockedExchange((volatile long*) atomic, value) != 0;
		//return _InterlockedExchange_acq((volatile long*) atomic, value) != 0;
	}

}