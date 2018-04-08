#include "M44.h"

#include <Core/Algorithm/Memory.h>
#include <Core/Common/Utility.h>

namespace math {

	M44::M44() {};

	M44::M44(const f32 values[4][4]) {
		Memcpy(data, values, 16 * sizeof(f32));
	}

	M44::M44(const M44& other) {
		Memcpy(data, other.data, 16 * sizeof(f32));
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
		Memcpy(data, rhs.data, 16 * sizeof(f32));
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

	M44 IdentityMatrix() {
		const f32 data[4][4] = {{1.f, 0.f, 0.f, 0.f},
															{0.f, 1.f, 0.f, 0.f},
															{0.f, 0.f, 1.f, 0.f},
															{0.f, 0.f, 0.f, 1.f}};
		return M44(data);
	}

	M44 TranslateMatrix(f32 tx, f32 ty, f32 tz) {
		f32 data[4][4] = {{1.f, 0.f, 0.f, 0.f},
												{0.f, 1.f, 0.f, 0.f},
												{0.f, 0.f, 1.f, 0.f},
												{ tx,  ty,  tz, 1.f}};
		return M44(data);
	}

	M44 ScaleMatrix(f32 sx, f32 sy, f32 sz) {
		f32 data[4][4] = {{ sx, 0.f, 0.f, 0.f},
												{0.f,  sy, 0.f, 0.f},
												{0.f, 0.f,  sz, 0.f},
												{0.f, 0.f, 0.f, 1.f}};
		return M44(data);
	}

	M44 RotateXMatrix(f32 rAngle) {

		f32 s = sin(rAngle);
		f32 c = cos(rAngle);

		f32 data[4][4] = {{1.f, 0.f, 0.f, 0.f},
												{0.f,   c,   s, 0.f},
												{0.f,  -s,   c, 0.f},
												{0.f, 0.f, 0.f, 1.f}};
		return M44(data);
	}

	M44 RotateYMatrix(f32 rAngle) {
		f32 s = sin(rAngle);
		f32 c = cos(rAngle);

		f32 data[4][4] = {{  c, 0.f,  -s, 0.f},
												{0.f, 1.f, 0.f, 0.f},
												{  s, 0.f,   c, 0.f},
												{0.f, 0.f, 0.f, 1.f}};
		return M44(data);
	}

	M44 RotateZMatrix(f32 rAngle) {
		f32 s = sin(rAngle);
		f32 c = cos(rAngle);

		f32 data[4][4] = {{  c,   s, 0.f, 0.f},
												{ -s,   c, 0.f, 0.f},
												{0.f, 0.f, 1.f, 0.f},
												{0.f, 0.f, 0.f, 1.f}};
		return M44(data);
	}

	M44 SymetricOrthogonalMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far) {

		f32 r = 2.f / (right - left);
		f32 u = 2.f / (top - bottom);
		f32 a = -2.f / (far - near);
		f32 b = -(2.f + near) / (far - near);

		f32 data[4][4] = {{r, 0.f, 0.f, 0.f},
												{0.f,   u, 0.f, 0.f},
												{0.f, 0.f,   a, 0.f},
												{0.f, 0.f,   b, 1.f}};

		return M44(data);
	}
}

