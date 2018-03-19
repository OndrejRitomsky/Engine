#include "GLWrapper.h"

#include <Extern/Include/GL/glew.h>

#include <Core/CoreAssert.h>

#include <stdio.h>

	namespace gl {
		//-------------------------------------------------------------------------
		static void GLAssert() {
#ifndef NDEBUG
			GLenum err;
			while ((err = glGetError()) != GL_NO_ERROR) {
				printf("Error gl assert %d \n", err);
				ASSERT(false);
			}
#endif
		}

		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		namespace program {
			//-----------------------------------------------------------------------
			u32 Create() {
				u32 program = glCreateProgram();
				GLAssert();
				return program;
			}

			//-----------------------------------------------------------------------
			void Use(u32 program) {
				glUseProgram((GLuint) program);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Delete(u32 program) {
				glDeleteProgram((GLuint) program);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void AttachShader(u32 program, u32 shader) {
				glAttachShader((GLuint) program, (GLuint) shader);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Link(u32 program) {
				glLinkProgram((GLuint) program);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			bool LinkSuccess(u32 program) {
				GLint success = GL_FALSE;
				glGetProgramiv((GLuint) program, GL_LINK_STATUS, &success);
				GLAssert();
				return success != 0;
			}

			//-----------------------------------------------------------------------
			bool Is(u32 handle) {
				u32 isProgram = glIsProgram((GLuint) handle);
				GLAssert();
				return isProgram != 0;
			}

			//-----------------------------------------------------------------------
			i32 InfoLog(u32 program, char* message, i32 size) {
				i32 realSize = 0;

				glGetProgramiv((GLuint) program, GL_INFO_LOG_LENGTH, &realSize);
				GLAssert();

				if (realSize > size)
					ASSERT(false); // not error but wont get the whole message

				i32 resultSize;
				glGetProgramInfoLog((GLuint) program, size, &resultSize, message);
				GLAssert();

				if (resultSize > 0 && resultSize < size) {
					message[resultSize] = 0;
					resultSize++;
				}

				return resultSize;
			}
		}


		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		namespace shader {
			//-----------------------------------------------------------------------
			u32 CreateVertex() {
				u32 shader = (u32) glCreateShader(GL_VERTEX_SHADER);
				GLAssert();
				return shader;
			}

			//-----------------------------------------------------------------------
			u32 CreateFragment() {
				u32 shader = (u32) glCreateShader(GL_FRAGMENT_SHADER);
				GLAssert();
				return shader;
			}

			//-----------------------------------------------------------------------
			void Compile(u32 shader, u32 count, const char** sources) {
				glShaderSource((GLuint) shader, count, sources, NULL);
				GLAssert();
				glCompileShader((GLuint) shader);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Delete(u32 shader) {
				glDeleteShader((GLuint) shader);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			bool CompileSuccess(u32 shader) {
				GLint success = GL_FALSE;
				glGetShaderiv((GLuint) shader, GL_COMPILE_STATUS, &success);
				GLAssert();
				return success != 0;
			}

			//-----------------------------------------------------------------------
			bool Is(u32 handle) {
				u32 result = glIsShader(handle);
				GLAssert();
				return result != 0;
			}

			//-----------------------------------------------------------------------
			i32 InfoLog(u32 shader, char* message, i32 size) {
				i32 realSize = 0;

				glGetShaderiv((GLuint) shader, GL_INFO_LOG_LENGTH, &realSize);
				GLAssert();

				if (realSize > size)
					ASSERT(false); // not error but wont get the whole message

				i32 resultSize;
				glGetShaderInfoLog((GLuint) shader, size, &resultSize, message);
				GLAssert();

				if (resultSize > 0 && resultSize < size) {
					message[resultSize] = 0;
					resultSize++;
				}

				return resultSize;
			}
		}

		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		namespace uniform {
			i32 Count(u32 program) {
				i32 count;
				glGetProgramiv((GLuint) program, GL_ACTIVE_UNIFORMS, &count);
				GLAssert();
				return count;
			}

			//-----------------------------------------------------------------------
			i32 Location(u32 program, const char* uniformName) {
				i32 location = (i32) glGetUniformLocation((GLuint) program, (const GLchar*) uniformName);
				GLAssert();
				return location;
			}

			//-----------------------------------------------------------------------
			void Info(u32 program, u32 index, char* name, i32 nameSize, i32& outNameSize, i32& outArraySize, u32& outGLType) {

				glGetActiveUniform(program, (GLuint) index, (GLint) nameSize, (GLint*) &outNameSize, (GLint*) &outArraySize, (GLenum*) &outGLType, (GLchar*) name);
				GLAssert();
				ASSERT(outArraySize == 1);

				if (outNameSize < 63) {
					outNameSize++;
				}
				else {
					ASSERT(false);
				}

				name[outNameSize - 1] = 0;
			}

			//-----------------------------------------------------------------------
			void Set1f(u32 location, f32 value) {
				glUniform1f(location, value);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Set2f(u32 location, const f32 values[2]) {
				glUniform2f(location, values[0], values[1]);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Set3f(u32 location, const f32 values[3]) {
				glUniform3f(location, values[0], values[1], values[2]);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void SetMatrix4f(u32 location, const f32 values[16]) {
				glUniformMatrix4fv(location, 1, GL_FALSE, values);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Set1i(u32 location, i32 value) {
				glUniform1i(location, value);
				GLAssert();
			}
		}

		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		namespace line {
			//-----------------------------------------------------------------------
			void Width(f32 width) {
				glLineWidth(width);
				GLAssert();
			}
		}

		//-------------------------------------------------------------------------
		//-------------------------------------------------------------------------
		namespace texture {
			//-----------------------------------------------------------------------
			u32 Create() {
				u32 id;
				glGenTextures(1, &id);
				GLAssert();
				return id;
			}

			//-----------------------------------------------------------------------
			void Delete(u32 texture) {
				glDeleteTextures(1, &texture);
			}

			//-----------------------------------------------------------------------
			void SetData(const void* data, u32 width, u32 height, bool generateMipMaps, render::TextureFormat format) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				GLAssert();
				if (generateMipMaps)
					glGenerateMipmap(GL_TEXTURE_2D);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void SetActiveTexture(u32 offset) {
				glActiveTexture(GL_TEXTURE0 + offset);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void Bind(u32 texture) {
				glBindTexture(GL_TEXTURE_2D, texture);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void Unbind() {
				glBindTexture(GL_TEXTURE_2D, 0);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			static i32 ConvertWrapParam(render::TextureWrapType wrap) {
				if (wrap == render::TextureWrapType::Repeat) {
					return GL_REPEAT;
				}

				ASSERT(false);
				return GL_REPEAT;
			}

			//-------------------------------------------------------------------------
			void SetParameterWrapST(render::TextureWrapType wrapS, render::TextureWrapType wrapT) {
				GLint wrapSParam = ConvertWrapParam(wrapS);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
				GLAssert();

				GLint wrapTParam = ConvertWrapParam(wrapT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			static i32 ConvertFilterParam(render::TextureFilterType filter) {
				if (filter == render::TextureFilterType::Linear) {
					return GL_LINEAR;
				} 
				else if (filter == render::TextureFilterType::Nearest) {
					return GL_NEAREST;
				}
				else if (filter == render::TextureFilterType::LinearMipMapLinear) {
					return GL_LINEAR_MIPMAP_LINEAR;
				}			

				ASSERT(false);
				return GL_LINEAR;
			}

			//-------------------------------------------------------------------------
			void SetParameterMinMagFilter(render::TextureFilterType minFilter, render::TextureFilterType magFilter) {
				GLint minParam = ConvertFilterParam(minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minParam);
				GLAssert();

				GLint maxParam = ConvertFilterParam(magFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxParam);
				GLAssert();
			}
		}

		namespace draw {
			//-----------------------------------------------------------------------
			static u32 ConvertIndexType(render::IndexType type) {
				u32 result = GL_UNSIGNED_SHORT + 2 * static_cast<u32>(type);
				ASSERT(result == GL_UNSIGNED_SHORT && type == render::IndexType::U16 ||
					result == GL_UNSIGNED_INT && type == render::IndexType::U32);

				return result;
			}

			//-----------------------------------------------------------------------
			void ClearColor(f32 color[4]) {
				glClearColor(color[0], color[1], color[2], color[3]);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void Clear(bool color, bool depth, bool stencil) {
				u32 c = GL_COLOR_BUFFER_BIT * static_cast<u32>(color);
				u32 d = GL_DEPTH_BUFFER_BIT * static_cast<u32>(depth);
				u32 s = GL_STENCIL_BUFFER_BIT * static_cast<u32>(stencil);
				glClear(c | d | s);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineArrays(u32 offset, u32 count) {
				glDrawArrays(GL_LINES, offset, count);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineLoopArrays(u32 offset, u32 count) {
				glDrawArrays(GL_LINE_LOOP, offset, count);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void TriangleArrays(u32 offset, u32 count) {
				glDrawArrays(GL_TRIANGLES, offset, count);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineElements(u32 indicesCount, render::IndexType type) {
				glDrawElements(GL_LINES, indicesCount, ConvertIndexType(type), 0);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineLoopElements(u32 indicesCount, render::IndexType type) {
				glDrawElements(GL_LINE_LOOP, indicesCount, ConvertIndexType(type), 0);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void TriangleElements(u32 indicesCount, render::IndexType type) {
				glDrawElements(GL_TRIANGLES, indicesCount, ConvertIndexType(type), 0);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
				glDrawElementsInstanced(GL_LINES, indicesCount, ConvertIndexType(type), 0, instancesCount);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void LineLoopElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
				glDrawElementsInstanced(GL_LINE_LOOP, indicesCount, ConvertIndexType(type), 0, instancesCount);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void TriangleElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
				glDrawElementsInstanced(GL_TRIANGLES, indicesCount, ConvertIndexType(type), 0, instancesCount);
				GLAssert();
			}
		}

		//-------------------------------------------------------------------------
		namespace vertex {
			//-------------------------------------------------------------------------
			void CreateArrayObjects(u32 count, u32 outIds[]) {
				glGenVertexArrays(count, outIds);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void CreateBuffers(u32 count, u32 outIds[]) {
				glGenBuffers(count, outIds);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void DeleteArrayObjects(u32 ids[], u32 count) {
				glDeleteVertexArrays(count, ids);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void DeleteBuffers(u32 ids[], u32 count) {
				glDeleteBuffers(count, ids);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void BindArrayObject(u32 id) {
				glBindVertexArray(id);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void BindArrayBuffer(u32 id) {
				glBindBuffer(GL_ARRAY_BUFFER, id);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void BindIndexArrayBuffer(u32 id) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void UnbindArrayObject() {
				glBindVertexArray(0);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void UnbindArrayBuffer() {
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void UnbindIndexBuffer() {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void ArrayBufferStaticData(const char* data, u32 size) {
				glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void ArrayBufferDynamicData(const char* data, u32 size) {
				glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void IndexArrayBufferStaticData(const char* data, u32 size) {
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
				GLAssert();
			}

			//-------------------------------------------------------------------------
			void IndexArrayBufferDynamicData(const char* data, u32 size) {
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void EnableAttributeArray(u32 index) {
				glEnableVertexAttribArray(index);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void AttributePointerFloat(u32 index, u32 size, u32 stride, u32 offset, bool normalize) {
				u64 off = offset;

				glVertexAttribPointer(index, size, GL_FLOAT, normalize, stride, (void*) off);
				GLAssert();
			}

			//-----------------------------------------------------------------------
			void AttributeDivisor(u32 index, u32 divisor) {
				glVertexAttribDivisor(index, divisor);
				GLAssert();
			}
		}
	}

