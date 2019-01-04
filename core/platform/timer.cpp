#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace core {
	static i64 CounterFrequency() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		return freq.QuadPart;
	}

	i64 CounterNano() {
		static i64 frequncy = CounterFrequency() / 1000000;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		return time.QuadPart / frequncy;
	}

	i64 CounterMicro() {
		static i64 frequncy = CounterFrequency() / 1000;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		return time.QuadPart / frequncy;
	}


	void Sleep(i32 miliseconds) {
		::Sleep(miliseconds);
	}

	void Timer::Start() {
		_start = CounterNano();
		_last = 0;
	}

	i64 Timer::Tick() {
		i64 prevLast = _last;
		_last = CounterNano() - _start;
		return _last - prevLast;
	}

	i64 Timer::Elapsed() {
		return CounterNano() - _start;
	}
}