#include "Thread.h"

#include "Core/CoreAssert.h"

#include <Windows.h>

namespace core {
	//---------------------------------------------------------------------------
	Thread::Thread() :
		_running(false),
		_handle(nullptr) {
	}

	//---------------------------------------------------------------------------
	Thread::~Thread() {
		ASSERT(!_running);
	}

	//---------------------------------------------------------------------------
	bool Thread::Create(Function fn, void* parameter) {
		if (_running)
			return false;

		// 1. child process cant interit, 2. stack size is by default 1page, 
		// 3. fn 4. parameter
		// 5 thread runs immediately, 6 out thread id
		_handle = CreateThread(NULL, 0, fn, parameter, 0, &_id);

		_running = _handle != nullptr;

		return _running;
	}

	//---------------------------------------------------------------------------
	bool Thread::Join() {
		if (!_running)
			return false;

		DWORD result = WaitForSingleObject(_handle, INFINITE);

		_running = result == WAIT_FAILED;

		ASSERT(!_running);
		return !_running;
	}

}
