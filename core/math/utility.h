#pragma once

#include "../common/types.h"

namespace core {

	f32 Cos(f32 radians);

	f32 Sin(f32 radians);

	f64 Cos(f64 radians);

	f64 Sin(f64 radians);

	f64 Deg2rad(f64 deg);

	f32 Sqrt(f32 value);

	f32 Lerp(f32 start, f32 stop, f32 ratio);



	inline f32 Lerp(f32 start, f32 stop, f32 ratio) {
		return start * (1.0f - ratio) + stop * ratio;
	}
}