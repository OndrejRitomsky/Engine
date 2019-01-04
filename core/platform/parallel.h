#pragma once


#include "parallel_types.h"

#include "../common/types.h"

namespace core {

	Event EventCreate(bool manual);

	void EventWait(Event evt);

	bool EventIsFinished(Event evt);

	void EventNotify(Event evt);

	void EventReset(Event evt);

	void EventClose(Event evt);



	void FutureInit(Future* future);

	bool FutureIsFinished(const Future* future);

	void FutureSetFinished(Future* future, void* res);

	void* FutureResult(const Future* future);

	void FutureWait(const Future* future);

	void FutureClose(Future* future);



	Semaphore SemaphoreCreate(u32 startCount, u32 maxCount);

	void SemaphoreWait(Semaphore semaphore);

	void SemaphoreRelease(Semaphore semaphore, u32 count);

	void SemaphoreClose(Semaphore semaphore);



	void ThreadSetCurrentToCore(int core);

	bool ThreadInit(Thread* thread, u32(*threadFunctio)(void*), void* parameter, int core);

	bool ThreadJoin(Thread* thread);

	bool ThreadIsRunning(const Thread* thread);

}




