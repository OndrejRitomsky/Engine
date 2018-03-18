#include "JobPool.h"

#include "Core/CoreAssert.h"

// @NOTES 

// @TODO Do padding for atomics, refactor -> performance tests needed!

// @TODO improvements
// Queue per phys. thread
// ? Waiting for task (task ID / task parent child)
// ? CV in queue for GetTaskWait (removes active waiting in workers) // This probably wont to with lockless queue

//?? Interface task for custom functions??? Wont do until needed, clean memory usage solution is way too uglier than using single void*
//   (can be private virtual with template creation and tasks memory could be pooled since tasks are created destructed in order)

namespace core {

	//---------------------------------------------------------------------------
  JobPool::JobPool() :
		_running(false) {
		_ending.store(false);
		_tasksPending.store(0);
	}

	//---------------------------------------------------------------------------
  JobPool::~JobPool() {
		ASSERT2(!_running, "JobPool DESTROY NOT CALLED"); 

		if (_running) { 
			Destroy();
		}
	}

	//---------------------------------------------------------------------------
	unsigned long JobPool::WorkerLoop(void* param) {
    JobPool* threadPool = (JobPool*) param;

		while (!threadPool->IsEnding()) {
			threadPool->TryDoOneTask();
		}

		return 0;
	}

	//---------------------------------------------------------------------------
	void JobPool::TryDoOneTask() {
    Job job;
		if (!TryGetTask(job))
			return;

    job.fn(job.param);
		DecreasePendingTasks();
	}

	//---------------------------------------------------------------------------
	void JobPool::Init() {
		for (u32 i = 0; i < THREAD_COUNT; ++i) {
			_workers[i].Create(WorkerLoop, this);
		}
		_running = true;
	}

	//---------------------------------------------------------------------------
	bool JobPool::TryAddTask(const JobFunction& function, void* param) {
		ASSERT(_running);
    JobPool::Job task;
		task.fn = function;
		task.param = param;
		
		// Pending tasks count has to be increased preemptively
		// so there is always constrain on runningTaskCount >= _queue._size + # of threads executing a task
		IncreasePendingTasks();
		bool ok = _queue.TryAdd(task);

		if (!ok)
			DecreasePendingTasks();

		return ok;
	}

	//---------------------------------------------------------------------------
	void JobPool::WaitAll() {
		ASSERT(_running);
		while (HasPendingTasks());
	}


	//---------------------------------------------------------------------------
	void JobPool::WaitAllAndWork() {
		ASSERT(_running);
		while (HasPendingTasks()) {
			TryDoOneTask();
		}
	}
	

	//---------------------------------------------------------------------------
	bool JobPool::TryGetTask(JobPool::Job& taskOut) {
		ASSERT(_running);
		return _queue.TryGet(taskOut);
	}

	//---------------------------------------------------------------------------
	bool JobPool::IsEmpty() {
		ASSERT(_running);
		return _queue.IsEmpty();
	}

	//---------------------------------------------------------------------------
	void JobPool::Destroy() {
		ASSERT(_running);
		ASSERT(IsEmpty());

		SignalEnd();

		bool ok = true;
		for (int i = 0; i < THREAD_COUNT; i++) {
			ok &= _workers[i].Join();
		}

		ASSERT(ok);
		ASSERT(_tasksPending.load() == 0);

		_running = false;
		_ending.store(false);
	}

	//---------------------------------------------------------------------------
	bool JobPool::IsEnding() {
		// this can be relaxed! No code / data needs "happens before" on WorkerLoop
		return _ending.load(std::memory_order_acquire); 
	}

	//---------------------------------------------------------------------------
	void JobPool::SignalEnd() {
		_ending.store(true, std::memory_order_release);
	}

	//---------------------------------------------------------------------------
	void JobPool::IncreasePendingTasks() {
		_tasksPending.fetch_add(1, std::memory_order_release);
	}

	//---------------------------------------------------------------------------
	void JobPool::DecreasePendingTasks() {
		_tasksPending.fetch_sub(1, std::memory_order_release);
	}

	//---------------------------------------------------------------------------
	bool JobPool::HasPendingTasks() {
		return _tasksPending.load(std::memory_order_acquire) > 0;
	}
}

