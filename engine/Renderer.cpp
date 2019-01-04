#include "Renderer.h"



#define _USE_MATH_DEFINES
#include <cmath>

#include <stdio.h>

#include <extern/include/gl/glew.h>
#include <extern/include/gl/wglew.h>

#include <core/common/debug.h>
#include <core/allocator/allocate.h>



#define WIN32_LEAN_AND_MEAN
#include <windows.h>


static bool FileOpen(const char* path, void** outHandle) {
	auto handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == handle)
		return false;

	*outHandle = handle;
	return true;

}

static bool FileRead(void* handle, char* data, u32 size) {
	HANDLE h = (HANDLE) handle;
	DWORD bytesToRead = size;
	DWORD bytesRead = 0;

	bool ok = false;
	while (true) {
		if (!::ReadFile(h, data + (size - bytesToRead), bytesToRead, &bytesRead, NULL))
			break;

		bytesToRead -= bytesRead;
		if (bytesToRead == 0 || bytesRead == 0)
			return true;
	}

	return false;
}



static char* FileOpenReadClose(const char* path, core::IAllocator* allocator, u32* outSize) {
	void* handle;
	if (!FileOpen(path, &handle))
		return nullptr;

	u32 fileSize = GetFileSize((HANDLE) handle, NULL);
	if (fileSize == INVALID_FILE_SIZE) {
		return nullptr;
	}

	char* data = (char*) core::Allocate(allocator, fileSize, 1);

	if (!FileRead(handle, data, fileSize)) {
		Deallocate(allocator, data);
		return nullptr;
	}

	data[fileSize - 1] = '\0';

	CloseHandle((HANDLE) handle);
	if (outSize)
		*outSize = fileSize;

	return  data;
}








bool Renderer_BoxShaderMaterialInit(BoxShaderMaterial* outBoxShaderMaterial, core::IAllocator* allocator);

f64 Internal_Deg2rad(f64 deg) {
	return deg * M_PI / 180.0;
}




bool CompileShader(const char* data, bool vert, u32* outShader) {
	GLuint shader = glCreateShader(vert ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("%s\n", infoLog);
		return false;
	}

	*outShader = shader;
	return true;
}


bool CreateProgram(const char* vert, const char* frag, u32* outProgram) {
	u32 vs, fs;
	if (!CompileShader(vert, true, &vs))
		return false;

	if (!CompileShader(frag, false, &fs))
		return false;

	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		return false;
	}

	*outProgram = program;
	return true;
}

bool RendererInit(Renderer* renderer, void* whdc, core::IAllocator* allocator) {
	static_assert(sizeof(HDC) == sizeof(void*), "Size missmatch");
	static_assert(sizeof(HGLRC) == sizeof(void*), "Size missmatch");

	HDC hdc = (HDC) whdc;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (nPixelFormat == 0)
		return false;

	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);

	if (!bResult)
		return false;

	HGLRC tempContext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, tempContext);

	GLenum err = glewInit();
	if (GLEW_OK != err)
		return false;

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	HGLRC rdc;

	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
		rdc = wglCreateContextAttribsARB(hdc, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hdc, rdc);
	}
	else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		rdc = tempContext;
	}

	//Checking GL version
	const GLubyte *GLVersionString = glGetString(GL_VERSION);

	//Or better yet, use the GL3 way to get the version number
	int OpenGLVersion[2];
	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

	if (!rdc) 
		return false;

	BoxShaderMaterial boxShaderMaterial;
	if (!Renderer_BoxShaderMaterialInit(&boxShaderMaterial, allocator))
		return false;


	renderer->renderDeviceContext = rdc;
	renderer->deviceContext = hdc;
	renderer->boxMaterial = boxShaderMaterial;

	RendererResize(renderer, 1024, 768);

	return true;
}


void RendererClear() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RendererDrawRectangle(Renderer* renderer, f32 posx, f32 posy, f32 sizex, f32 sizey, f32 r, f32 g, f32 b) {
	BoxShaderMaterial* fontMaterial = &renderer->boxMaterial;

	glUseProgram(fontMaterial->program);
	glBindVertexArray(fontMaterial->vao);
	glUniform3f(fontMaterial->colorUniLocation, r, g, b);

	M44 mat = MatrixScale(sizex, sizey, 1) * MatrixTranslate(posx - renderer->width / 2, renderer->height - posy - renderer->height / 2, 1) * renderer->viewMatrix;

	glUniformMatrix4fv(fontMaterial->mvp, 1, false, mat.data[0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



void RendererResize(Renderer* renderer, i32 width, i32 height) {
	renderer->width = width;
	renderer->height = height;

	renderer->viewMatrix = MatrixSymetricOrthogonal((f32) renderer->width, (f32) renderer->height, 0.0f, 100.0f);

	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
}

bool Renderer_BoxShaderMaterialInit(BoxShaderMaterial* outBoxShaderMaterial, core::IAllocator* allocator) {
	u32 prog;

	char* vert = FileOpenReadClose("box_vs.glsl", allocator, nullptr);
	char* frag = FileOpenReadClose("box_fs.glsl", allocator, nullptr);

	if (!vert || !frag)
		return false;

	if (!CreateProgram(vert, frag, &prog)) {
		Deallocate(allocator, frag);
		Deallocate(allocator, vert);
		return false;
	}

	Deallocate(allocator, frag);
	Deallocate(allocator, vert);

	f32 v = 0.5f;
	float vertices[] = {
		-v, v, 
		v,  v, 
		-v, -v,
		v,  -v
	};


	GLuint VBO;
	GLuint VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*) 0);
	glEnableVertexAttribArray(0);


	outBoxShaderMaterial->program = prog;
	outBoxShaderMaterial->mvp = glGetUniformLocation(prog, "mvp");
	outBoxShaderMaterial->colorUniLocation = glGetUniformLocation(prog, "color");	
	outBoxShaderMaterial->vao = VAO;
	outBoxShaderMaterial->vbo[0] = VBO;
	outBoxShaderMaterial->vboCount = 1;

	return true;
}

