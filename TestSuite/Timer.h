#pragma once

#include <chrono>
#include <ctime>

class Timer {
public:
  void Start() {
		auto now = std::chrono::high_resolution_clock::now();
		_start = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
  }

	long long int End() {
		auto now = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count();
    return end - _start;
  }

private:
  long long int _start;
};
