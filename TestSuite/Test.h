#pragma once

#include "Console/ConsoleColor.h"

#include "TestGroup.h"
#include "TestConfig.h"

#include <stdio.h>

static float FLOAT_EPSILON = 0.000000001f;

namespace __test {
	//-----------------------------------------------------------------------------
	static void Log(bool result, const char* function, const char *text) {	
		printf("  %s", function);

		static const char dots[] = "................................................................................";
		// -2 first last spaces, -6 for FAILED, -2 for offset, +1 for zero char in fn name
		int n = 80 - 2 - 6 - 2 - (int) strlen(function) + 1;
		printf(" %.*s ", n, dots);

		if (result)
			console::SetTextColor(console::GREEN_TEXT);
		else
			console::SetTextColor(console::RED_TEXT);

		printf("%s\n", result ? "OK" : "FAILED");

	
		if (!result) {
			console::SetTextColor(console::GREY_TEXT);
			printf("    %s \n\n", text);
		}
		console::SetTextColor(console::WHITE_TEXT);
	}

	//-----------------------------------------------------------------------------
	static bool FloatEqual(float a, float b) {
		return (a - b < FLOAT_EPSILON) && (b - a < FLOAT_EPSILON);
	}
}

//-----------------------------------------------------------------------------
#define UpdateUnit(testGroup, correct) { \
	testGroup.IncTested(); \
	if (correct) testGroup.IncCorrects(); \
}

//-----------------------------------------------------------------------------
#define LogResult(testGroup, correct, text) { \
	if (testGroup.GetConfig().logSuccess || !(correct)) __test::Log((correct), __FUNCTION__, (text)); \
	char c; \
	if (testGroup.GetConfig().stopOnErorr && !(correct)) scanf_s("%c", &c, 1u); \
}

//-----------------------------------------------------------------------------
#define TestAssert(testGroup, expression, text) { \
	bool excCought = false; \
	try { expression; } \
	catch (const char*) { excCought = true; } \
	UpdateUnit(testGroup, excCought) \
	LogResult(testGroup, excCought, (text)) \
}

//-----------------------------------------------------------------------------
#define TestTrue(testGroup, a, text) { \
	UpdateUnit(testGroup, a) \
	LogResult(testGroup, a, (text)) \
}

//-----------------------------------------------------------------------------
#define TestLower(testGroup, a, b, text) { \
	bool correct = a < b; \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}

//-----------------------------------------------------------------------------
#define TestLowerEqual(testGroup, a, b, text) { \
	bool correct = a <= b; \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}

//-----------------------------------------------------------------------------
#define TestEqual(testGroup, a, b, text) { \
	bool correct = a == b; \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}

//-----------------------------------------------------------------------------
#define TestNotEqual(testGroup, a, b, text) { \
	bool correct = a != b; \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}

//-----------------------------------------------------------------------------
#define TestFloatEqual(testGroup, a, b, text) { \
	bool correct = __test::FloatEqual(a, b); \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}

//-----------------------------------------------------------------------------
#define TestArrayEqual(testGroup, a, b, count, text) { \
	bool correct = true; \
	for (int i = 0; i < count; ++i) { \
		correct &= (a)[i] == (b)[i]; \
	} \
	UpdateUnit(testGroup, correct) \
	LogResult(testGroup, correct, (text)) \
}
