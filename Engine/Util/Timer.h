#pragma once

#include <Core/Common/Types.h>

// @TODO This has low resolution !!!

class Timer {
public:
	Timer() {};
	~Timer() {};

	void Start();
	f32 Tick();
	f32 Elapsed();

private:

	unsigned int start;
	unsigned int last;
};

