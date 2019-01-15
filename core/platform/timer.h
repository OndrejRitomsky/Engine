#pragma once

#include "../common/types.h"

namespace core {
	void Sleep(i32 miliseconds);

	f64 CounterNano();
	f64 CounterMicro();

	// currently nano
	class Timer {
	public:
		void Start();
		f64 Tick();
		f64 Elapsed();
	private:
		f64 _start;
		f64 _last;
	};
}


