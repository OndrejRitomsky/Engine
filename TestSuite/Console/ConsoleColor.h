#pragma once

#include <windows.h>

namespace console
{
	static const unsigned int GREEN_TEXT = 0x0A;
	static const unsigned int RED_TEXT = 0x0C;
	static const unsigned int WHITE_TEXT = 0x0F;
	static const unsigned int GREY_TEXT = 0x07;

	inline void SetTextColor(unsigned int color) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	}

	inline void ResetTextColor() {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE_TEXT);
	}
}