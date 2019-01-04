#pragma once

#include "../common/types.h"

namespace core {
	void Sleep(i32 miliseconds);

	i64 CounterNano();
	i64 CounterMicro();

	// currently nano
	class Timer {
	public:
		void Start();
		i64 Tick();
		i64 Elapsed();
	private:
		i64 _start;
		i64 _last;
	};
}


