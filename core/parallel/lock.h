#pragma once

#include "../platform/parallel_types.h"
#include "../platform/parallel.h"

namespace core {
	// LockType void lock(), void unlock(), 
	template<typename LockType>
	class LockGuard {
	public:
		LockGuard(LockType& lock);

		LockGuard(const LockGuard& oth) = delete;
		LockGuard& operator=(const LockGuard& rhs) = delete;

		~LockGuard();
	private:
		LockType & _lock;
	};

	class SpinLock {
	public:
		SpinLock();

		SpinLock(const SpinLock& oth) = delete;
		SpinLock& operator=(const SpinLock& rhs) = delete;

		void lock();
		void unlock();

	private:
		AtomicBool _atomic;
	};


	template<typename LockType>
	class ConditionVariable {
		ConditionVariable(LockType& lock);

		void Wait();
		void Notify();

	private:
		bool _validWake;
		Event _event;
		LockType& _lock;
	};



	template<typename LockType>
	ConditionVariable<LockType>::ConditionVariable(LockType& lock) :
		_lock(lock),
		_validWake(false) {

		_event = EventCreate(true);
	}

	template<typename LockType>
	void ConditionVariable<LockType>::Wait() {
		bool awakened = false;
		do {
			_lock.unlock(); // must have been locked
			EventWait(_event);
			_lock.lock(); // spin locking until the awakened one releases the lock
			if (locked)
				awakened = _validWake;

		} while (!awakened);

		// stil locked
		_validWake = false;


		EventReset(_event);
	}

	template<typename LockType>
	void ConditionVariable<LockType>::Notify() {
		_lock.lock();
		_validWake = true;
		EventNotify(_event); // can be outside of lock?
		_lock.unlock();
	}



	template<typename LockType>
	LockGuard<LockType>::LockGuard(LockType& lock) :
		_lock(lock) {

		_lock.lock();
	}

	template<typename LockType>
	LockGuard<LockType>::~LockGuard() {
		_lock.unlock();
	}


}