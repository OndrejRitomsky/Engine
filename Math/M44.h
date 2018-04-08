#pragma once

#include <Core/Common/Types.h>

#include "Math/V2.h"

// @TODO revisit, this is old code
// @TODO revisit, this is old code

namespace math {

	struct M44 {
		f32 data[4][4];

		M44();
		M44(const f32 values[4][4]);
		M44(const M44& other);
		M44(f32 value);

		const M44& operator=(const M44& rhs);

		void Inverse();
		void Transpose();

		M44 Inversed();
		M44 Transposed();
	};

	M44 operator*(const M44& lhs, const M44& rhs);

	V2 operator*(const V2& lhs, const M44& rhs);

	M44 IdentityMatrix();

	M44 TranslateMatrix(f32 tx, f32 ty, f32 tz);

	M44 ScaleMatrix(f32 sx, f32 sy, f32 sz);

	M44 RotateXMatrix(f32 rAngle);

	M44 RotateYMatrix(f32 rAngle);

	M44 RotateZMatrix(f32 rAngle);

	M44 SymetricOrthogonalMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
}

