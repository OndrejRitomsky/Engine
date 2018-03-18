#pragma once

#include <atomic>

namespace core {

	class SpinLock {
	public:
		SpinLock();

		SpinLock(const SpinLock& oth) = delete;
		SpinLock& operator=(const SpinLock& rhs) = delete;

		void lock();
		void unlock();

	private:
		std::atomic_flag _atomicFlag;
	};

}
