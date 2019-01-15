#include "job_pool.h"

#include "../platform/atomic.h"
#include "../platform/parallel.h"

#include "../common/debug.h"

#include "../allocator/allocate.h"

#include "../collection/ringbuffer.inl"


// @NOTES 

// @TODO Do padding for atomics, refactor -> performance tests needed!

// @TODO improvements
// Queue per phys. thread
// ? Waiting for task (task ID / task parent child)
// ? CV in queue for GetTaskWait (removes active waiting in workers) // This probably wont to with lockless queue

//?? Interface task for custom functions??? Wont do until needed, clean memory usage solution is way too uglier than using single void*
//   (can be private virtual with template creation and tasks memory could be pooled since tasks are created destructed in order)

namespace core {

	JobPool::JobPool() :
		_running(false),
		_workers(nullptr),
		_allocator(nullptr),
		_workersCount(0) {

		AtomicBoolWrite(&_ending, false);
	}

	JobPool::~JobPool() {
		assert(!_running);

		if (_running) {
			Destroy();
			Deallocate(_allocator, _workers);
		}
	}

	u32 JobPool::WorkerLoop(void* param) {
		JobPool* threadPool = (JobPool*) param;

		while (!threadPool->IsEnding()) {
			bool did = threadPool->TryDoOneTask();

			if (!did) {
				threadPool->WaitNewTask();
			}
		}

		return 0;
	}

	void JobPool::WaitNewTask() {
		SemaphoreWait(_semaphore);
	}

	bool JobPool::TryDoOneTask() {
		Job job;
		{
			LockGuard<SpinLock> lockGuard(_lock);
			if (!_queue.TryGet(job))
				return false;
		}

		void* res = job.fn(job.param);

		if (job.future) {
			FutureSetFinished(job.future, res);
		}

		return true;
	}

	void JobPool::Init(IAllocator* allocator, u32 count) {
		_running = true;
		_semaphore = SemaphoreCreate(0, RingBuffer<Job>::CAPACITY);
		_allocator = allocator;

		_workers = (Thread*) Allocate(allocator, sizeof(Thread) * count, alignof(Thread));
		_workersCount = count;

		ThreadSetCurrentToCore(0);

		i32 mul = 1;
		if (count < 4)
			mul = 2;

		for (u32 i = 0; i < _workersCount; ++i) {
			ThreadInit(&_workers[i], JobPool::WorkerLoop, this, mul *(i + 1));
		}
	}

	bool JobPool::TryAddTask(const JobFunction& function, void* param, Future* future) {
		assert(_running);
		Job task;
		task.fn = function;
		task.param = param;
		task.future = future;

		// Pending tasks count has to be increased preemptively
		// so there is always constrain on runningTaskCount >= _queue._size + # of threads executing a task
		bool ok;
		{
			LockGuard<SpinLock> lockGuard(_lock);
			ok = _queue.TryAdd(task);
		}

		if (ok) {
			SemaphoreRelease(_semaphore, 1);
		}

		return ok;
	}

	int JobPool::TryAddTasks(const JobFunction& function, void** params, int count) {
		return 0;

		assert(_running);

		Job task;
		task.fn = function;

		i32 i = 0;

		for (; i < count; ++i) {
			task.param = params[i];

			LockGuard<SpinLock> lockGuard(_lock);
			if (!_queue.TryAdd(task))
				break;
		}

		return i;
	}

	void JobPool::WaitForAndWork(const Future* futures, u32 count) {
		assert(_running);

		for (u32 i = 0; i < count; i++) {
			while (!FutureIsFinished(&futures[i])) {
				if (!TryDoOneTask()) {
					FutureWait(&futures[i]);
				}
			}
		}
	}

	bool JobPool::IsEmpty() {
		assert(_running);
		LockGuard<SpinLock> lockGuard(_lock);
		return _queue.IsEmpty();
	}

	void JobPool::Destroy() {
		assert(_running);
		assert(IsEmpty());

		AtomicBoolWriteRel(&_ending, true);

		SemaphoreRelease(_semaphore, _workersCount);

		bool ok = true;
		for (u32 i = 0; i < _workersCount; i++) {
			ok &= ThreadJoin(&_workers[i]);
		}

		assert(ok);
		_running = false;
		AtomicBoolWrite(&_ending, false);
	}


	bool JobPool::IsEnding() {
		// this can be relaxed! No code / data needs "happens before" on WorkerLoop
		return AtomicBoolReadAcq(&_ending);
	}

}