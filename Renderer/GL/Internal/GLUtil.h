#pragma once

#include <Core/Types.h>

#include "Renderer/API/Command/Uniform.h"

namespace gl {
	namespace util {
		u32 CompileVertexShader(const char* source);
		u32 CompileFragmentShader(const char* source);

		u32 CreateProgram(u32 vertexShader, u32 fragmentShader);

		render::UniformType GLTypeToUniformType(u32 glType);
	}
}
