#include "Utility.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace core {
	f32 Cos(f32 radians) {
		return (f32) cos(radians);
	}

	f32 Sin(f32 radians) {
		return (f32) sin(radians);
	}


	f64 Cos(f64 radians) {
		return cos(radians);
	}

	f64 Sin(f64 radians) {
		return sin(radians);
	}

	f64 Deg2rad(f64 deg) {
		return deg * M_PI / 180.;
	}

	f32 Sqrt(f32 value) {
		return sqrtf(value);
	}
}