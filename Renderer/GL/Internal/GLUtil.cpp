#include "GLUtil.h"

#include <Extern/Include/GL/glew.h>

#include <Core/CoreAssert.h>

#include "Renderer/GL/Internal/GLWrapper.h"

#include <cstdio>

namespace gl {
	namespace util {

		//-----------------------------------------------------------------------
		u32 CreateProgram(u32 vertexShader, u32 fragmentShader) {
			u32 program = program::Create();

			program::AttachShader(program, vertexShader);
			program::AttachShader(program, fragmentShader);
			program::Link(program);

			if (!program::LinkSuccess(program)) {
				const i32 size = 1024;
				static char buffer[size];
				program::InfoLog(program, buffer, size);
				printf("%s\n", buffer);
				ASSERT(false);
			}

			return program;
		}

		static void LogShaderError(u32 shader) {
			const i32 size = 1024;
			static char buffer[size];
			shader::InfoLog(shader, buffer, size);
			printf("%s\n", buffer);
			ASSERT(false);
		}

		//-----------------------------------------------------------------------
		u32 CompileVertexShader(const char* source) {
			u32 shader = shader::CreateVertex();
			shader::Compile(shader, 1, &source);

			if (!shader::CompileSuccess(shader))
				LogShaderError(shader);

			return shader;
		}

		//-----------------------------------------------------------------------
		u32 CompileFragmentShader(const char* source) {
			u32 shader = shader::CreateFragment();
			shader::Compile(shader, 1, &source);

			if (!shader::CompileSuccess(shader))
				LogShaderError(shader);

			return shader;
		}

		//-----------------------------------------------------------------------
		render::UniformType GLTypeToUniformType(u32 glType) {
			render::UniformType uniformType;

			switch ((GLenum) glType) {
			case GL_FLOAT: uniformType = render::UniformType::FLOAT; break;
			case GL_FLOAT_VEC2: uniformType = render::UniformType::VECTOR2; break;
			case GL_FLOAT_VEC3: uniformType = render::UniformType::VECTOR3; break;
			case GL_FLOAT_MAT4: uniformType = render::UniformType::MATRIX; break;
			case GL_SAMPLER_2D: uniformType = render::UniformType::SAMPLER2D; break;
			case GL_INT: uniformType = render::UniformType::INT; break;

			default: ASSERT(false); break;
			}

			return uniformType;
		}
	}
}
