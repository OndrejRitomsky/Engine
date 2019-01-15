#pragma once

//#include <math.h>

#include "utility.h"
#include "../common/types.h"



struct V2 {
	f32 x, y;
	//union {
	//	struct {
	//		f32 x, y;
	//	};
	//	f32 data[2];
	//};

	V2();
	V2(f32 X, f32 Y);
	explicit V2(f32 value);
	V2(const V2& other);

	V2& operator=(const V2& rhs);

	V2& operator+=(const V2& rhs);
	V2& operator-=(const V2& rhs);
	V2& operator*=(f32 rhs);
	V2& operator/=(f32 rhs);

	V2& operator-();

	f32 operator[](int i) const;
	f32& operator[](int i);

	void Normalize();

	f32 Magnitude();
	f32 MagnitudeSqr();
	f32 Distance(const V2& rhs) const;
	f32 DistanceSqr(const V2& rhs) const;

	f32 Dot(const V2& rhs) const;

	V2 Reflect(const V2& normal) const;

	V2 Normalized() const;
};

inline V2 operator+(const V2& lhs, const V2& rhs);
inline V2 operator-(const V2& lhs, const V2& rhs);
inline V2 operator*(const V2& lhs, const V2& rhs);
inline V2 operator*(const V2& lhs, f32 rhs);
inline V2 operator*(f32 lhs, const V2& rhs);
inline V2 operator/(const V2& lhs, f32 rhs);

inline V2::V2() :
	x(0),
	y(0) {
}

inline V2::V2(f32 value) :
	x(value),
	y(value) {
}

inline V2::V2(f32 X, f32 Y) :
	x(X),
	y(Y) {
}

inline V2::V2(const V2& other) :
	x(other.x),
	y(other.y) {
}

inline V2& V2::operator=(const V2& rhs) {
	x = rhs.x;
	y = rhs.y;
	return *this;
}

inline V2& V2::operator+=(const V2& rhs) {
	x += rhs.x;
	y += rhs.y;
	return *this;
}

inline V2& V2::operator-=(const V2& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}

inline V2& V2::operator*=(f32 rhs) {
	x *= rhs;
	y *= rhs;
	return *this;
}

inline V2& V2::operator/=(f32 rhs) {
	x /= rhs;
	y /= rhs;
	return *this;
}

inline V2& V2::operator-() {
	V2 res;
	res.x = -x;
	res.y = -y;
	return res;
}

inline f32 V2::operator[](int i) const {
	return (&x)[i];
}

inline f32& V2::operator[](int i) {
	return (&x)[i];
}

inline void V2::Normalize() {
	f32 length = Magnitude();
	x /= length;
	y /= length;
}

inline f32 V2::Magnitude() {
	return core::Sqrt(x*x + y*y);
}

inline f32 V2::MagnitudeSqr() {
	return x*x + y*y;
}

inline f32 V2::Distance(const V2& rhs) const {
	f32 dx = x - rhs.x;
	f32 dy = y - rhs.y;
	return core::Sqrt(dx*dx + dy*dy);
}

inline f32 V2::DistanceSqr(const V2 &rhs) const {
	f32 dx = x - rhs.x;
	f32 dy = y - rhs.y;
	return dx*dx + dy*dy;
}

inline f32 V2::Dot(const V2& rhs) const {
	return x * rhs.x + y * rhs.y;
}

inline V2 V2::Reflect(const V2& normal) const {
	V2 result = *this - 2.f * Dot(normal) * normal;
	return result;
}

inline V2 V2::Normalized() const {
	V2 result(*this);
	result.Normalize();
	return result;
}

inline V2 operator+(const V2& lhs, const V2& rhs) {
	V2 result(lhs);
	result += rhs;
	return result;
}

inline V2 operator-(const V2& lhs, const V2& rhs) {
	V2 result(lhs);
	result -= rhs;
	return result;
}

inline V2 operator*(const V2& lhs, const V2& rhs) {
	V2 result(lhs);
	result.x *= rhs.x;
	result.y *= rhs.y;
	return result;
}

inline V2 operator*(const V2& lhs, f32 rhs) {
	V2 result(lhs);
	result *= rhs;
	return result;
}

inline V2 operator*(f32 lhs, const V2& rhs) {
	V2 result(rhs);
	result *= lhs;
	return result;
}

inline V2 operator/(const V2& lhs, f32 rhs) {
	V2 result(lhs);
	result /= rhs;
	return result;
}

