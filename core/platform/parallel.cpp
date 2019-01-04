#include "parallel.h"


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "../common/debug.h"


namespace core {

// -------------------------- FUTURE ------------------------------------------

	void FutureInit(Future* future) {
		future->_evt = EventCreate(true);
		future->_result = nullptr;
	}


	bool FutureIsFinished(const Future* future) {
		return EventIsFinished(future->_evt);
	}


	void FutureSetFinished(Future* future, void* res) {
		future->_result = res;
		EventNotify(future->_evt);
	}


	void* FutureResult(const Future* future) {
		return future->_result;
	}


	void FutureWait(const Future* future) {
		EventWait(future->_evt);
	}


	void FutureClose(Future* future) {
		EventClose(future->_evt);
	}


// -------------------------- EVENT -------------------------------------------

	static_assert(sizeof(Event) == sizeof(HANDLE), "Size missmatch");


	Event EventCreate(bool manual) {
		DWORD flags = manual ? CREATE_EVENT_MANUAL_RESET : 0;
		return (Event) CreateEventEx(NULL, NULL, flags, EVENT_MODIFY_STATE | SYNCHRONIZE);
	}


	void EventWait(Event evt) {
		auto e = WaitForSingleObjectEx((HANDLE) evt, INFINITE, false);
		if (e != WAIT_OBJECT_0) {
			auto ee = GetLastError();
			ASSERT(false);
		}
	}

	bool EventIsFinished(Event evt) {
		return WaitForSingleObjectEx((HANDLE) evt, 0, false) == WAIT_OBJECT_0;
	}

	void EventNotify(Event evt) {
		SetEvent((HANDLE) evt);
	}

	void EventReset(Event evt) {
		auto e = ResetEvent((HANDLE) evt);
		if (!e) {
			auto ee = GetLastError();
			ASSERT(false);
		}
	}

	void EventClose(Event evt) {
		CloseHandle((HANDLE) evt);
	}




// -------------------------- SEMAPHORE ---------------------------------------

	static_assert(sizeof(Semaphore) == sizeof(HANDLE), "Size missmatch");


	Semaphore SemaphoreCreate(u32 startCount, u32 maxCount) {
		return (Semaphore) CreateSemaphoreExA(NULL, startCount, maxCount, "", 0, SEMAPHORE_MODIFY_STATE | SYNCHRONIZE);
	}


	void SemaphoreWait(Semaphore semaphore) {
		auto e = WaitForSingleObjectEx((HANDLE) semaphore, INFINITE, false);
		if (e != WAIT_OBJECT_0) {
			auto ee = GetLastError();
			assert(false);
		}
	}

	void SemaphoreRelease(Semaphore semaphore, u32 count) {
		if (!ReleaseSemaphore(semaphore, count, NULL)) {
			assert(false);
		}
	}

	void SemaphoreClose(Semaphore semaphore) {
		CloseHandle((HANDLE) semaphore);
	}


// -------------------------- THREAD ------------------------------------------


	void ThreadSetCurrentToCore(int core) {
		WORD aff = 1 << core;
		auto val = SetThreadAffinityMask(GetCurrentThread(), aff);

		if (val == 0) {
			assert(false);
		}
	}

	bool ThreadInit(Thread* thread, u32(*threadFunctio)(void*), void* parameter, int core) {
		// 1. child process cant interit, 2. stack size is by default 1page, 
		// 3. fn 4. parameter
		// 5 thread runs immediately, 6 out thread id
		thread->_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadFunctio, parameter, 0, (LPDWORD) &thread->_id);

		if (thread->_handle == INVALID_HANDLE_VALUE)
			return false;

		assert(core < 8);

		WORD aff = 1 << core;
		auto val = SetThreadAffinityMask(thread->_handle, aff);

		if (val == 0) {
			assert(false);
		}

		thread->_running = thread->_handle != INVALID_HANDLE_VALUE;

		return thread->_running;
	}


	bool ThreadJoin(Thread* thread) {
		if (!thread->_running)
			return false;

		DWORD result = WaitForSingleObject(thread->_handle, INFINITE);

		thread->_running = result == WAIT_FAILED;

		assert(!thread->_running);
		return !thread->_running;
	}

	bool ThreadIsRunning(const Thread* thread) {
		return thread->_running;
	}
}