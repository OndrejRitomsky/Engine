#pragma once

#include "Timer.h"
#include <cstdio>

#define PerformanceTestStart(perfTest) \
			Timer _timer; _timer.Start(); \

#define PerformanceTestStop(perfTest) { \
			long long int time = _timer.End(); \
			perfTest.sum += time; perfTest.count++; \
			if (time < perfTest.best) perfTest.best = time; \
			if (time > perfTest.worst) perfTest.worst = time; \
		}

#define PerformanceTestStop2(perfTest, res) { \
			long long int time = _timer.End(); \
			perfTest.result = res; \
			perfTest.sum += time; perfTest.count++; \
			if (time < perfTest.best) {perfTest.best = time;} \
			if (time > perfTest.worst) {perfTest.worst = time;} \
	}

#define PerformanceTest() printf("Function: %s \n", __FUNCTION__);