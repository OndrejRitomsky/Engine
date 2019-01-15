#include "lock.h"

#include "../platform/atomic.h"


namespace core {

	SpinLock::SpinLock() {
		AtomicBoolWrite(&_atomic, false);
	}


	void SpinLock::lock() {
		while (AtomicBoolExchangeAcq(&_atomic, true));
	}


	void SpinLock::unlock() {
		AtomicBoolWriteRel(&_atomic, false);
	}

}