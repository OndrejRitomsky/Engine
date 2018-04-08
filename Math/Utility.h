#pragma once

#include <Core/Common/Types.h>

namespace math {

	f32 Cos(f32 radians);

	f32 Sin(f32 radians);

	f64 Cos(f64 radians);

	f64 Sin(f64 radians);

	f64 Deg2rad(f64 deg);


	template<typename T>
	bool IsPowerOfTwo(T x);

	template<typename T>
	T Lerp(const T& start, const T& stop, f32 ratio);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------


	//----------------------------------------------------------------------------
	template<typename T>
	inline bool IsPowerOfTwo(T x) {
		return x != 0 && !(x & (x - 1));
	}

	//----------------------------------------------------------------------------
	template<typename T>
	T Lerp(const T& start, const T& stop, f32 ratio) {
		return start * (1.0f - ratio) + stop * ratio;

	}

}