#pragma once

#include "lock.h"

#include "../common/types.h"
#include "../collection/ringbuffer.h"
#include "../platform/parallel_types.h"

// @TODO usage

// @TODO !!!!!!!!!!!!! Futures should be pooled inside



namespace core {
	struct IAllocator;
	struct Thread;

	class JobPool {
	public:
		typedef void*(*JobFunction)(void*);

		__declspec(align(64))
			struct Job {
			JobFunction fn;
			void* param;
			Future* future;
		};

	public:
		JobPool();
		JobPool(const JobPool& oth) = delete;
		JobPool& operator=(const JobPool& rhs) = delete;

		~JobPool();

		void Init(IAllocator* allocator, u32 count);

		bool TryAddTask(const JobFunction& function, void* param, Future* future);

		int TryAddTasks(const JobFunction& function, void** params, int count);

		void WaitForAndWork(const Future* futures, u32 count);

		// Informs threads to stop (after they finish their current task) and joins them (=waits while they cleanup)
		// Pending tasks will not be finished
		void Destroy();

	private:
		static u32 WorkerLoop(void* param);

		void WaitNewTask();

		bool TryDoOneTask();

		bool IsEnding();
		bool IsEmpty();

	private:
		// @TODO need more info
		__declspec(align(64)) AtomicBool _ending;
		__declspec(align(64)) SpinLock _lock;
		Semaphore _semaphore;

	private:
		bool _running;

		u32 _workersCount;

		Thread* _workers;
		IAllocator* _allocator;

		RingBuffer<Job> _queue;
	};

}

