#pragma once

#include <core/common/types.h>
#include <core/common/debug.h>

#include "../api/render_resources.h"
#include "../api/command/uniform.h"

#include <Extern/Include/GL/glew.h>

// @TODO maybe change converters so they are not called from here


#ifndef NDEBUG
#include <stdio.h>
#endif


void GlViewPort(i32 width, i32 height);

void GLAssert();

u32 GlProgram(u32 vertexShader, u32 fragmentShader);

void GlProgramUse(u32 program);
void GlProgramDelete(u32 program);
void GlProgramAttachShader(u32 program, u32 shader);
void GlProgramLink(u32 program);

bool GlProgramLinkSuccess(u32 program);
bool GlProgramIs(u32 handle);

i32 GlProgramInfoLog(u32 program, char* message, i32 size);

u32 GlShaderVertex();
u32 GlShaderFragment();


bool GlShaderCompile(u32 shader, const char* const* sources, u32 sourcesCount);

void GlShaderDelete(u32 shader);

bool GlShaderCompileSuccess(u32 shader);
bool GlShaderIs(u32 handle);

void GlLogShaderError(u32 shader);

i32 GlUniformCount(u32 program);
i32 GlUniformLocation(u32 program, const char* uniformName);

void GlUniformInfo(u32 program, u32 index, char* name, i32 nameSize, i32& outNameSize, i32& outArraySize, u32& outGLType);
void GlUniformSet1f(u32 location, f32 value);
void GlUniformSet2f(u32 location, const f32 values[2]);
void GlUniformSet3f(u32 location, const f32 values[3]);
void GlUniformSetMatrix4f(u32 location, const f32 values[16]);
void GlUniformSet1i(u32 location, i32 value);

void GlLineWidth(f32 width);


u32 GlTexture();
void GlTextureDelete(u32 texture);

void GlTextureSetData(const void* data, u32 width, u32 height, bool generateMipMaps, render::TextureFormat format);
void GlTextureSetActiveTexture(u32 offset);
void GlTextureBind(u32 texture);
void GlTextureUnbind();
void GlTextureSetParameterWrapST(render::TextureWrapType wrapS, render::TextureWrapType wrapT);
void GlTextureSetParameterMinMagFilter(render::TextureFilterType minFilter, render::TextureFilterType magFilter);


void GlClearColor(f32 color[4]);
void GlClear(bool color, bool depth, bool stencil);

void GlDrawLineArrays(u32 offset, u32 count);
void GlDrawLineLoopArrays(u32 offset, u32 count);
void GlDrawTriangleArrays(u32 offset, u32 count);

void GlDrawLineElements(u32 indicesCount, render::IndexType type);
void GlDrawLineLoopElements(u32 indicesCount, render::IndexType type);
void GlDrawTriangleElements(u32 indicesCount, render::IndexType type);

void GlDrawLineElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);
void GlDrawLineLoopElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);
void GlDrawTriangleElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);

void GlVertexArrays(u32 count, u32 outIds[]);
void GlVertexArraysDelete(u32 ids[], u32 count);
void GlVertexArrayBind(u32 id);
void GlVertexArrayUnbind();

void GlBuffers(u32 count, u32 outIds[]);
void GlBuffersDelete(u32 ids[], u32 count);

void GlArrayBufferBind(u32 id);
void GlArrayBufferUnbind();
void GlArrayBufferStaticData(const void* data, u32 size);
void GlArrayBufferDynamicData(const void* data, u32 size);

void GlIndexArrayBufferBind(u32 id);
void GlIndexBufferUnbind();
void GlIndexArrayBufferStaticData(const void* data, u32 size);
void GlIndexArrayBufferDynamicData(const void* data, u32 size);

void GlEnableAttributeArray(u32 index);
void GlAttributePointerFloat(u32 index, u32 size, u32 stride, u32 offset, bool normalize);
void GlAttributeDivisor(u32 index, u32 divisor);

render::UniformType GlTypeToUniformType(u32 glType);


inline void GlViewPort(i32 width, i32 height) {
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
}

inline void GLAssert() {
#ifndef NDEBUG
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		//printf("Error gl assert %d \n", err);
		ASSERT(false);
	}
#endif
}

inline u32 GlProgram(u32 vertexShader, u32 fragmentShader) {
	u32 program = glCreateProgram();
	GLAssert();

	GlProgramAttachShader(program, vertexShader);
	GlProgramAttachShader(program, fragmentShader);
	GlProgramLink(program);

	if (!GlProgramLinkSuccess(program)) {
		const i32 size = 1024;
		static char buffer[size];
		GlProgramInfoLog(program, buffer, size);
		//printf("%s\n", buffer);
		ASSERT(false);
	}

	return program;
}

inline void GlProgramUse(u32 program) {
	glUseProgram((GLuint) program);
	GLAssert();
}

inline void GlProgramDelete(u32 program) {
	glDeleteProgram((GLuint) program);
	GLAssert();
}

inline void GlProgramAttachShader(u32 program, u32 shader) {
	glAttachShader((GLuint) program, (GLuint) shader);
	GLAssert();
}

inline void GlProgramLink(u32 program) {
	glLinkProgram((GLuint) program);
	GLAssert();
}

inline bool GlProgramLinkSuccess(u32 program) {
	GLint success = GL_FALSE;
	glGetProgramiv((GLuint) program, GL_LINK_STATUS, &success);
	GLAssert();
	return success != 0;
}

inline bool GlProgramIs(u32 handle) {
	u32 isProgram = glIsProgram((GLuint) handle);
	GLAssert();
	return isProgram != 0;
}

inline i32 GlProgramInfoLog(u32 program, char* message, i32 size) {
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


inline u32 GlShaderVertex() {
	u32 shader = (u32) glCreateShader(GL_VERTEX_SHADER);
	GLAssert();
	return shader;
}

inline u32 GlShaderFragment() {
	u32 shader = (u32) glCreateShader(GL_FRAGMENT_SHADER);
	GLAssert();
	return shader;
}

inline bool GlShaderCompile(u32 shader, const char* const* sources, u32 sourcesCount) {
	glShaderSource((GLuint) shader, sourcesCount, sources, NULL);
	GLAssert();
	glCompileShader((GLuint) shader);
	GLAssert();

	if (!GlShaderCompileSuccess(shader)) {
		GlLogShaderError(shader);
		return false;
	}

	return true;
}

inline void GlShaderDelete(u32 shader) {
	glDeleteShader((GLuint) shader);
	GLAssert();
}

inline bool GlShaderCompileSuccess(u32 shader) {
	GLint success = GL_FALSE;
	glGetShaderiv((GLuint) shader, GL_COMPILE_STATUS, &success);
	GLAssert();
	return success != 0;
}

inline bool GlShaderIs(u32 handle) {
	u32 result = glIsShader(handle);
	GLAssert();
	return result != 0;
}

// @rename ?
inline i32 GlUniformCount(u32 program) {
	i32 count;
	glGetProgramiv((GLuint) program, GL_ACTIVE_UNIFORMS, &count);
	GLAssert();
	return count;
}

inline i32 GlUniformLocation(u32 program, const char* uniformName) {
	i32 location = (i32) glGetUniformLocation((GLuint) program, (const GLchar*) uniformName);
	GLAssert();
	return location;
}

inline void GlUniformInfo(u32 program, u32 index, char* name, i32 nameSize, i32& outNameSize, i32& outArraySize, u32& outGLType) {
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

inline void GlUniformSet1f(u32 location, f32 value) {
	glUniform1f(location, value);
	GLAssert();
}

inline void GlUniformSet2f(u32 location, const f32 values[2]) {
	glUniform2f(location, values[0], values[1]);
	GLAssert();
}

inline void GlUniformSet3f(u32 location, const f32 values[3]) {
	glUniform3f(location, values[0], values[1], values[2]);
	GLAssert();
}

inline void GlUniformSetMatrix4f(u32 location, const f32 values[16]) {
	glUniformMatrix4fv(location, 1, GL_FALSE, values);
	GLAssert();
}

inline void GlUniformSet1i(u32 location, i32 value) {
	glUniform1i(location, value);
	GLAssert();
}


inline void GlLineWidth(f32 width) {
	glLineWidth(width);
	GLAssert();
}


inline u32 GlTexture() {
	u32 id;
	glGenTextures(1, &id);
	GLAssert();
	return id;
}

inline void GlTextureDelete(u32 texture) {
	glDeleteTextures(1, &texture);
}

inline void GlTextureSetData(const void* data, u32 width, u32 height, bool generateMipMaps, render::TextureFormat format) {
	GLint glformat;
	if (format == render::TextureFormat::RGBA) glformat = GL_RGBA;
	else if (format == render::TextureFormat::RGB) glformat = GL_RGB;
	else ASSERT(false);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, glformat, GL_UNSIGNED_BYTE, data);
	GLAssert();
	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
	GLAssert();
}

inline void GlTextureSetActiveTexture(u32 offset) {
	glActiveTexture(GL_TEXTURE0 + offset);
	GLAssert();
}

inline void GlTextureBind(u32 texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	GLAssert();
}

inline void GlTextureUnbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	GLAssert();
}

inline i32 GlTextureConvertWrapParam(render::TextureWrapType wrap) {
	if (wrap == render::TextureWrapType::Repeat) {
		return GL_REPEAT;
	}

	ASSERT(false);
	return GL_REPEAT;
}

inline void GlTextureSetParameterWrapST(render::TextureWrapType wrapS, render::TextureWrapType wrapT) {
	GLint wrapSParam = GlTextureConvertWrapParam(wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapSParam);
	GLAssert();

	GLint wrapTParam = GlTextureConvertWrapParam(wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTParam);
	GLAssert();
}

inline i32 GlTextureConvertFilterParam(render::TextureFilterType filter) {
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

inline void GlTextureSetParameterMinMagFilter(render::TextureFilterType minFilter, render::TextureFilterType magFilter) {
	GLint minParam = GlTextureConvertFilterParam(minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minParam);
	GLAssert();

	GLint maxParam = GlTextureConvertFilterParam(magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxParam);
	GLAssert();
}


inline u32 GlConvertIndexType(render::IndexType type) {
	u32 result = GL_UNSIGNED_SHORT + 2 * static_cast<u32>(type);
	ASSERT(result == GL_UNSIGNED_SHORT && type == render::IndexType::U16 ||
		result == GL_UNSIGNED_INT && type == render::IndexType::U32);

	return result;
}

inline void GlClearColor(f32 color[4]) {
	glClearColor(color[0], color[1], color[2], color[3]);
	GLAssert();
}

inline void GlClear(bool color, bool depth, bool stencil) {
	u32 c = GL_COLOR_BUFFER_BIT * static_cast<u32>(color);
	u32 d = GL_DEPTH_BUFFER_BIT * static_cast<u32>(depth);
	u32 s = GL_STENCIL_BUFFER_BIT * static_cast<u32>(stencil);
	glClear(c | d | s);
	GLAssert();
}

inline void GlDrawLineArrays(u32 offset, u32 count) {
	glDrawArrays(GL_LINES, offset, count);
	GLAssert();
}

inline void GlDrawLineLoopArrays(u32 offset, u32 count) {
	glDrawArrays(GL_LINE_LOOP, offset, count);
	GLAssert();
}

inline void GlDrawTriangleArrays(u32 offset, u32 count) {
	glDrawArrays(GL_TRIANGLES, offset, count);
	GLAssert();
}

inline void GlDrawLineElements(u32 indicesCount, render::IndexType type) {
	glDrawElements(GL_LINES, indicesCount, GlConvertIndexType(type), 0);
	GLAssert();
}

inline void GlDrawLineLoopElements(u32 indicesCount, render::IndexType type) {
	glDrawElements(GL_LINE_LOOP, indicesCount, GlConvertIndexType(type), 0);
	GLAssert();
}

inline void GlDrawTriangleElements(u32 indicesCount, render::IndexType type) {
	glDrawElements(GL_TRIANGLES, indicesCount, GlConvertIndexType(type), 0);
	GLAssert();
}

inline void GlDrawLineElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
	glDrawElementsInstanced(GL_LINES, indicesCount, GlConvertIndexType(type), 0, instancesCount);
	GLAssert();
}

inline void GlDrawLineLoopElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
	glDrawElementsInstanced(GL_LINE_LOOP, indicesCount, GlConvertIndexType(type), 0, instancesCount);
	GLAssert();
}

inline void GlDrawTriangleElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount) {
	glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GlConvertIndexType(type), 0, instancesCount);
	GLAssert();
}

inline void GlVertexArrays(u32 count, u32 outIds[]) {
	glGenVertexArrays(count, outIds);
	GLAssert();
}

inline void GlVertexArraysDelete(u32 ids[], u32 count) {
	glDeleteVertexArrays(count, ids);
	GLAssert();
}

inline void GlVertexArrayBind(u32 id) {
	glBindVertexArray(id);
	GLAssert();
}

inline void GlVertexArrayUnbind() {
	glBindVertexArray(0);
	GLAssert();
}

inline void GlBuffers(u32 count, u32 outIds[]) {
	glGenBuffers(count, outIds);
	GLAssert();
}

inline void GlBuffersDelete(u32 ids[], u32 count) {
	glDeleteBuffers(count, ids);
	GLAssert();
}

inline void GlArrayBufferBind(u32 id) {
	glBindBuffer(GL_ARRAY_BUFFER, id);
	GLAssert();
}

inline void GlArrayBufferUnbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLAssert();
}

inline void GlArrayBufferStaticData(const void* data, u32 size) {
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	GLAssert();
}

inline void GlArrayBufferDynamicData(const void* data, u32 size) {
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	GLAssert();
}

inline void GlIndexArrayBufferBind(u32 id) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	GLAssert();
}

inline void GlIndexBufferUnbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLAssert();
}

inline void GlIndexArrayBufferStaticData(const void* data, u32 size) {
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	GLAssert();
}

inline void GlIndexArrayBufferDynamicData(const void* data, u32 size) {
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	GLAssert();
}

inline void GlEnableAttributeArray(u32 index) {
	glEnableVertexAttribArray(index);
	GLAssert();
}

inline void GlAttributePointerFloat(u32 index, u32 size, u32 stride, u32 offset, bool normalize) {
	u64 off = offset;

	glVertexAttribPointer(index, size, GL_FLOAT, normalize, stride, (void*) off);
	GLAssert();
}

inline void GlAttributeDivisor(u32 index, u32 divisor) {
	glVertexAttribDivisor(index, divisor);
	GLAssert();
}


inline void GlLogShaderError(u32 shader) {
	const i32 size = 1024;
	static char buffer[size];

	i32 realSize = 0;

	glGetShaderiv((GLuint) shader, GL_INFO_LOG_LENGTH, &realSize);
	GLAssert();

	if (realSize > size)
		ASSERT(false); // not error but wont get the whole message

	i32 resultSize;
	glGetShaderInfoLog((GLuint) shader, size, &resultSize, buffer);
	GLAssert();

	if (resultSize > 0 && resultSize < size) {
		buffer[resultSize] = 0;
		resultSize++;
	}

	//printf("%s\n", buffer);
	ASSERT(false);
}

inline render::UniformType GlTypeToUniformType(u32 glType) {
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
