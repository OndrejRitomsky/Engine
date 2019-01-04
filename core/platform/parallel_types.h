#pragma once

#include "../common/types.h"

namespace core {

	typedef volatile i32 AtomicI32;

	typedef volatile i32 AtomicBool;

	typedef void* Event;

	typedef void* Semaphore;

	struct Future {
		Event _evt;
		void* _result;
	};

	struct Thread {
		bool _running;
		u32 _id;
		void* _handle;
	};
}