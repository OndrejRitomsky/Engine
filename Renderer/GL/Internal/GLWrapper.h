#pragma once

#include <Core/types.h>
#include "Renderer/API/Resources/Texture.h"
#include "Renderer/API/Resources/Buffer.h"

namespace gl {
	namespace program {
		u32 Create();

		void Use(u32 program);
		void Delete(u32 program);
		void AttachShader(u32 program, u32 shader);
		void Link(u32 program);

		bool LinkSuccess(u32 program);
		bool Is(u32 handle);

		i32 InfoLog(u32 program, char* message, i32 size);
	}


	namespace shader {
		u32 CreateVertex();
		u32 CreateFragment();

		void Compile(u32 shader, u32 count, const char** sources);
		void Delete(u32 shader);

		bool CompileSuccess(u32 shader);
		bool Is(u32 handle);

		i32 InfoLog(u32 shader, char* message, i32 size);
	}


	namespace uniform {
		i32 Count(u32 program);
		i32 Location(u32 program, const char* uniformName);
		
		void Info(u32 program, u32 index, char* name, i32 nameSize, i32& outNameSize, i32& outArraySize, u32& outGLType);

		void Set1f(u32 location, f32 value);
		void Set2f(u32 location, f32 values[2]);
		void Set3f(u32 location, f32 values[3]);
		void SetMatrix4f(u32 location, f32 values[16]);
		void Set1i(u32 location, i32 value);
	}

	namespace line {
		void Width(f32 width);
	}

	namespace texture {
		u32 Create();

		void Delete(u32 texture);

		void SetData(const char* data, u32 width, u32 height, bool generateMipMaps, render::TextureFormat format);

		void SetActiveTexture(u32 offset);
		void Bind(u32 texture);
		void Unbind();

		void SetParameterWrapST(render::TextureWrapType wrapS, render::TextureWrapType wrapT);
		void SetParameterMinMagFilter(render::TextureFilterType minFilter, render::TextureFilterType magFilter);
	}

	namespace draw {
		void ClearColor(f32 color[4]);
		void Clear(bool color, bool depth, bool stencil);

		void LineArrays(u32 offset, u32 count);
		void LineLoopArrays(u32 offset, u32 count);
		void TriangleArrays(u32 offset, u32 count);

		void LineElements(u32 indicesCount, render::IndexType type);
		void LineLoopElements(u32 indicesCount, render::IndexType type);
		void TriangleElements(u32 indicesCount, render::IndexType type);

		void LineElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);
		void LineLoopElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);
		void TriangleElementsInstanced(u32 indicesCount, render::IndexType type, u32 instancesCount);
	}

	namespace vertex {
		void CreateArrayObjects(u32 count, u32 outIds[]);
		void CreateBuffers(u32 count, u32 outIds[]);
		
		void DeleteArrayObjects(u32 ids[], u32 count);
		void DeleteBuffers(u32 ids[], u32 count);

		void BindArrayObject(u32 id);
		void BindArrayBuffer(u32 id);
		void BindIndexArrayBuffer(u32 id);

		void UnbindArrayObject();
		void UnbindArrayBuffer();
		void UnbindIndexBuffer();

		void ArrayBufferStaticData(const char* data, u32 size);
		void ArrayBufferDynamicData(const char* data, u32 size);

		void IndexArrayBufferStaticData(const char* data, u32 size);
		void IndexArrayBufferDynamicData(const char* data, u32 size);

		void EnableAttributeArray(u32 index);
		void AttributePointerFloat(u32 index, u32 size, u32 stride, u32 offset, bool normalize);
		void AttributeDivisor(u32 index, u32 divisor);
	}
}
