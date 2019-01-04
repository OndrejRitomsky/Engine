#pragma once

#include <core/common/types.h>
#include <core/math/m44.h>
#include <core/math/v2.h>

namespace core {
	struct IAllocator;
}

struct ShaderMaterial {
	u8 vboCount;
	u32 program;
	u32 vao;
	u32 vbo[4];
};


struct BoxShaderMaterial : ShaderMaterial {
	i32 colorUniLocation;
	i32 mvp;
};

struct Renderer {
	i32 width;
	i32 height;
	void* deviceContext;
	void* renderDeviceContext;
	BoxShaderMaterial boxMaterial;
	M44 viewMatrix;
};


bool RendererInit(Renderer* renderer, void* whdc, core::IAllocator* allocator);

void RendererClear();

void RendererResize(Renderer* renderer, i32 width, i32 height);


void RendererDrawRectangle(Renderer* renderer, f32 posx, f32 posy, f32 sizex, f32 sizey, f32 r, f32 g, f32 b);


