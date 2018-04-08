#include "Timer.h"

#include <Extern/Include/SDL/SDL.h>


void Timer::Start() {
	start = SDL_GetTicks();
	last = 0;
}

f32 Timer::Tick() {
	unsigned int time = SDL_GetTicks() - start;
	f32 delta = (time - last) / 1000.f;
	last = time;

	return delta;
}

f32 Timer::Elapsed() {
	return (SDL_GetTicks() - start) / 1000.f;
}

