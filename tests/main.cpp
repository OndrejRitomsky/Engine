

#define TEST_IMPLEMENTATION
#include "test.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void ConsoleSetStyle(void*, TestConsoleStyle style) {
	const unsigned int GREEN_TEXT = 0x0A;
	const unsigned int RED_TEXT = 0x0C;
	const unsigned int GREY_TEXT = 0x07;

	switch (style) {
	case TEST_CONSOLE_TEXT_DEFAULT: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREY_TEXT); break;
	case TEST_CONSOLE_TEXT_SUCCESS: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREEN_TEXT); break;
	case TEST_CONSOLE_TEXT_FAIL: SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED_TEXT); break;
	case TEST_CONSOLE_INVALID:
	default:
		break;
	}
}

double Timer(void* timeContext) {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart / *(double*) timeContext;
}

double Frequency() {
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return li.QuadPart / 1000000.0;
}

void* operator new(size_t size) {
	return malloc(size);
}

void operator delete(void* ptr) {
	free(ptr);
}



int main(int argc, char* args[]) {
	double freq = Frequency();

	TestConfig config;
	config.SetConsoleStyle = ConsoleSetStyle;
	config.Time = Timer;
	config.stopOnEnd = false;
	config.stopOnError = false;
	config.perfStopOnEnd = true;
	config.floatEpsilon = 1e-10f;
	config.doubleEpsilon = 1e-20;
	config.styleContext = nullptr;
	config.timeContext = &freq;
	TestSetConfig(&config);
	bool ok = TestRun();
	TestRunPerf();


	//TestDeinit();
	return ok;
}