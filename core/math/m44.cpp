#include "m44.h"

#include "../algorithm/cstring.h"
#include "../common/utility.h"

M44::M44() {};

M44::M44(const f32 values[16]) {
	core::Memcpy(data, values, 16 * sizeof(f32));
}

M44::M44(const M44& other) {
	core::Memcpy(data, other.data, 16 * sizeof(f32));
}

M44::M44(f32 value) {
	for (int i = 0; i < 4; i++) {
		data[i][0] = value;
		data[i][1] = value;
		data[i][2] = value;
		data[i][3] = value;
	}
}

const M44& M44::operator=(const M44& rhs) {
	core::Memcpy(data, rhs.data, 16 * sizeof(f32));
	return *this;
}

void M44::Inverse() {
	f32 a = 4 + 5;
}

void M44::Transpose() {
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 4; j++) {
			core::Swap(data[i][j], data[j][i]);
		}
	}
}

M44 M44::Inversed() {
	return M44(*this);
}

M44 M44::Transposed() {
	M44 result(*this);
	result.Transpose();
	return result;
}

M44 operator*(const M44& lhs, const M44& rhs) {
	M44 result(0.f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.data[i][j] += lhs.data[i][k] * rhs.data[k][j];
			}
		}
	}
	return result;
}

V2 operator*(const V2& lhs, const M44& rhs) {
	V2 result(0.f);
	result[0] += lhs[0] * rhs.data[0][0] + lhs[1] * rhs.data[0][1] + rhs.data[0][3];
	result[1] += lhs[0] * rhs.data[1][0] + lhs[1] * rhs.data[1][1] + rhs.data[1][3];
	return result;
}

M44 MatrixIdentity() {
	float data[16] = {1.f, 0.f, 0.f, 0.f,
	                  0.f, 1.f, 0.f, 0.f,
	                  0.f, 0.f, 1.f, 0.f,
	                  0.f, 0.f, 0.f, 1.f};
	return M44(data);
}

M44 MatrixTranslate(f32 tx, f32 ty, f32 tz) {
	float data[16] = {1.f, 0.f, 0.f, 0.f,
	                  0.f, 1.f, 0.f, 0.f,
	                  0.f, 0.f, 1.f, 0.f,
	                  tx,  ty,  tz,  1.f};
	return M44(data);
}

M44 MatrixScale(f32 sx, f32 sy, f32 sz) {
	float data[16] = { sx, 0.f, 0.f, 0.f,
	                  0.f,  sy, 0.f, 0.f,
	                  0.f, 0.f,  sz, 0.f,
	                  0.f, 0.f, 0.f, 1.f};
	return M44(data);
}

M44 MatrixRotateX(f32 rAngle) {
	f32 s = core::Sin(rAngle);
	f32 c = core::Cos(rAngle);

	float data[16] = {1.f, 0.f, 0.f, 0.f,
	                  0.f,   c,   s, 0.f,
	                  0.f,  -s,   c, 0.f,
	                  0.f, 0.f, 0.f, 1.f};
	return M44(data);
}

M44 MatrixRotateY(f32 rAngle) {
	f32 s = core::Sin(rAngle);
	f32 c = core::Cos(rAngle);

	float data[16] = {  c, 0.f,  -s, 0.f,
	                  0.f, 1.f, 0.f, 0.f,
	                    s, 0.f,   c, 0.f,
	                  0.f, 0.f, 0.f, 1.f};
	return M44(data);
}

M44 MatrixRotateZ(f32 rAngle) {
	f32 s = core::Sin(rAngle);
	f32 c = core::Cos(rAngle);

	float data[16] = {  c,   s, 0.f, 0.f,
	                   -s,   c, 0.f, 0.f,
	                  0.f, 0.f, 1.f, 0.f,
	                  0.f, 0.f, 0.f, 1.f};
	return M44(data);
}

/*
inline M44 MatrixRotateAxis(const V3& axis, f32 rAngle) {
	f32 s = core::Sin(rAngle);
	f32 c = core::Cos(rAngle);

	f32 rs = 1.0f - s;
	f32 rc = 1.0f - c;

	f32 x = axis.x;
	f32 y = axis.y;
	f32 z = axis.z;
	// @todo clean
	M44 mat = {c + x * x*rc, x*y*rc + z * s, x*z*rc - y * s, 0.f,
		x*y*rc - z * s, c + y * y*rc, y*z*rc + x * s, 0.f,
		x*z*rc + y * s, y*z*rc - x * s, c + z * z*rc, 0.f,
		0.f, 0.f, 0.f, 1.f};

	return M44(data);
}*/

M44 MatrixRotateAngles(f32 pitch, f32 yaw, f32 roll) {

	f32 s1 = core::Sin(pitch);
	f32 c1 = core::Cos(pitch);

	f32 s2 = core::Sin(yaw);
	f32 c2 = core::Cos(yaw);

	f32 s3 = core::Sin(roll);
	f32 c3 = core::Cos(roll);

	// @todo clean

	
	float data[16] = { c2*c3, c1*s3 + c3*s1*s2, s1*s3 - c1*c3*s2, 0.f,
	                  -c2*s3, c1*c3 - s1*s2*s3, c3*s1 + c1*s2*s3, 0.f,
	                      s2,           -c2*s1,            c1*c2, 0.f,
	                     0.f,              0.f,              0.f, 1.f};

	return M44(data);
}


M44 MatrixRotateAngles(f32 pitch, f32 yaw) {

	f32 s1 = core::Sin(pitch);
	f32 c1 = core::Cos(pitch);

	f32 s2 = core::Sin(yaw);
	f32 c2 = core::Cos(yaw);

	// @todo clean
	
	float data[16] = { c2,  s1*s2, -c1*s2, 0.f,
	                    0,     c1,     s1, 0.f,
	                   s2, -c2*s1,  c1*c2, 0.f,
	                  0.f,    0.f,    0.f, 1.f};
	
	return M44(data);
}

M44 MatrixSymetricOrthogonal(f32 width, f32 height, f32 nr, f32 fr) {
	f32 r = 2.f / (width);
	f32 u = 2.f / (height);
	f32 a = -2.f / (fr - nr);
	f32 b = -(2.f + nr) / (fr - nr);

	float data[16] = {r,   0.f, 0.f, 0.f,
	                  0.f,   u, 0.f, 0.f,
	                  0.f, 0.f,   a, 0.f,
	                  0.f, 0.f,   b, 1.f};
	return M44(data);
}
