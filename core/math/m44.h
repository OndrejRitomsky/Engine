#pragma once

#include "../common/types.h"
#include "v2.h"


// @TODO revisit, this is old code
// @TODO revisit, this is old code

struct M44 {
	f32 data[4][4];

	M44();
	M44(const f32 values[16]);
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

M44 MatrixIdentity();

M44 MatrixTranslate(f32 tx, f32 ty, f32 tz);

M44 MatrixScale(f32 sx, f32 sy, f32 sz);

M44 MatrixRotateX(f32 rAngle);

M44 MatrixRotateY(f32 rAngle);

M44 MatrixRotateZ(f32 rAngle);

//M44 MatrixRotateAxis(const V3& axis, f32 rAngle);

M44 MatrixRotateAngles(f32 pitch, f32 yaw, f32 roll);

M44 MatrixSymetricOrthogonal(f32 width, f32 height, f32 nr, f32 fr);