#include "SpinLock.h"

#include <utility>

namespace core {
	//---------------------------------------------------------------------------
	SpinLock::SpinLock() {
		_atomicFlag.clear(); // _atomicFlag(ATOMIC_FLAG_INIT), MSVC problem?
	}

	//---------------------------------------------------------------------------
	void SpinLock::lock() {
		while (_atomicFlag.test_and_set(std::memory_order_acquire));
	}

	//---------------------------------------------------------------------------
	void SpinLock::unlock() {
		_atomicFlag.clear(std::memory_order_release);
	}
}
