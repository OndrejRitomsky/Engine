#pragma once

#include <core/common/types.h>

namespace render {

	enum class UniformType : u32 {
		MATERIAL = 0,
		SAMPLER2D,
		MATRIX,
		VECTOR2,
		VECTOR3,
		FLOAT,
		INT
	};

	//UniformType type; -> renderer has this information (at least now)

	struct UniformHeader {
		h64 nameHash;
	};

	struct IntUniform : UniformHeader {
		i32	value;
	};

	struct FloatUniform : UniformHeader {
		f32	value;
	};

	struct Vector2Uniform : UniformHeader {
		f32	values[2];
	};

	struct Vector3Uniform : UniformHeader {
		f32	values[3];
	};

	struct MatrixUniform : UniformHeader {
		f32	values[16];
	};

	struct Sampler2DUniform : UniformHeader {
		u32 handle;
	};
}