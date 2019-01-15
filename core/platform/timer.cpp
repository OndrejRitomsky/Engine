#include "Timer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace core {
	static i64 CounterFrequency() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		return freq.QuadPart;
	}

	f64 CounterNano() {
		static f64 frequncy = CounterFrequency() / 1000000000.0;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		return time.QuadPart / frequncy;
	}

	f64 CounterMicro() {
		static f64 frequncy = CounterFrequency() / 1000000.0;

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

	f64 Timer::Tick() {
		f64 prevLast = _last;
		_last = CounterNano() - _start;
		return _last - prevLast;
	}

	f64 Timer::Elapsed() {
		return CounterNano() - _start;
	}
}