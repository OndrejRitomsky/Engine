#pragma once


#include "Core/Types.h"

#include "Core/Parallel/Collection/ThreadSafeFixedQueue.h"
#include "Core/Parallel/Locks/SpinLock.h"
#include "Core/Parallel/Thread.h"

#include "Core/Collection/Function.h"


	// ----------------------- USAGE -------------------------------------
	// //core::JobPool pool;
	// pool.Init();
	// 
	// auto fn = core::ThreadPool::TaskFunction::Make<Func>();
	// for (int i = 0; i < 128; i++) {
	// 	pool.TryAddTask(fn, (void*) i);
	// }
	// pool.WaitAllAndWork(); // or pool.WaitAll();
	//
	// // ... do more ....
	// pool.Destroy();

namespace core {
	class JobPool {
	private:
		// @TODO query this number from OS!!! This is important as Spinlock used IS SLOW when there is more threads than physical ones
		static const u32 THREAD_COUNT = 3; 
		static const u16 TASKS_MAX_COUNT = 1024;  // @?TODO create dynamic locked queue with POD data only

	public:
		typedef Function<void(void*)> JobFunction;

	private:
		struct Job {
      JobFunction fn;
			void* param;
		};

	public:
		JobPool();
		JobPool(const JobPool& oth) = delete;
		JobPool& operator=(const JobPool& rhs) = delete;

		~JobPool();
		
		void Init();
		
		bool TryAddTask(const JobFunction& function, void* param);

		// Returns when all pending tasks are FINISHED, waits actively!!
		void WaitAll();

		// Returns when all pending tasks are FINISHED, helps with tasks while waiting!!
		void WaitAllAndWork();

		// Informs threads to stop (after they finish their current task) and joins them (=waits while they cleanup)
		// Pending tasks will not be finished
		void Destroy();

	private:
		static unsigned long WorkerLoop(void* param);

		void TryDoOneTask();

		void SignalEnd();

		bool IsEnding();
		bool IsEmpty();
		bool TryGetTask(Job& taskOut); 

		void IncreasePendingTasks();
		void DecreasePendingTasks();
		bool HasPendingTasks();

	private:
		std::atomic_bool _ending;       // This might need cache line padding
		//char __padding[64 - sizeof(std::atomic_bool)];
		std::atomic_int  _tasksPending; // This might need cache line padding
		//char __padding2[64 - sizeof(std::atomic_int)];

	private:
		bool _running;
		Thread _workers[THREAD_COUNT];
		ThreadSafeFixedQueue<Job, core::SpinLock, TASKS_MAX_COUNT> _queue;
	};
}