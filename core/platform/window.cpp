#include "window.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <Windowsx.h>


#include "../allocator/allocate.h"
#include "../collection/buffer.h"
#include "../collection/array.h"
#include "../collection/array.inl"


namespace core {

	struct W32WindowSize {
		int width;
		int height;
	};

	// Its class for array and buffer destructors!
	class Window {
	public:
		Window() {}
		~Window() {}

		void Init(IAllocator* all) {
			input.Init(all);
			eventsBuffer.Init(all);
			mousePos.Init(all);
			allocator = all;
		}

		void* deviceContext;
		void* hwnd;

		const char* name;
		Buffer eventsBuffer;
		Array<PlatformMousePos> mousePos;
		Array<PlatformKeyType> input;
		IAllocator* allocator;
	};


	W32WindowSize W32_GetWindowSize(HWND window);

	LRESULT CALLBACK W32_WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);


	void WindowSetCursor(PlatformCursorType cursor) {
		switch (cursor) {
		case PLATFORM_CURSOR_ARROW:
			SetCursor(::LoadCursor(NULL, IDC_ARROW));
			break;
		case PLATFORM_CURSOR_IBEAM:
			SetCursor(::LoadCursor(NULL, IDC_IBEAM));
			break;
		case PLATFORM_CURSOR_HAND:
			SetCursor(::LoadCursor(NULL, IDC_HAND));
			break;
		case PLATFORM_CURSOR_VERTICAL:
			SetCursor(::LoadCursor(NULL, IDC_SIZENS));
			break;
		case PLATFORM_CURSOR_HORIZONTAL:
			SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			break;
		case PLATFORM_CURSOR_TL_BR:
			SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
			break;
		case PLATFORM_CURSOR_TR_BL:
			SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
			break;
		case PLATFORM_CURSOR_INVALID:
		default:
			break;
		}
	}


	Window* WindowInit(IAllocator* allocator, u32 width, u32 height, const char* name) {
		void* windowBuffer = Allocate(allocator, sizeof(Window), alignof(Window));
		Window* window = Placement(windowBuffer) Window();
		window->Init(allocator);


		WNDCLASSA windowClass = {};
		// OWNDC is not shared with anyone
		windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = W32_WindowCallback;
		windowClass.hInstance = GetModuleHandle(NULL);
		windowClass.lpszClassName = name;
		// windowClass.cbWndExtra 

		window->name = name;

		HCURSOR hCursorHelp = ::LoadCursor(NULL, IDC_HELP);

		if (!RegisterClassA(&windowClass)) {
			window->~Window();
			Deallocate(allocator, window);
			return nullptr;
		}

		HWND hwnd = CreateWindowExA(0, windowClass.lpszClassName, name,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, windowClass.hInstance, window);

		if (!hwnd) {
			window->~Window();
			Deallocate(allocator, window);
			return nullptr;
		}

		window->hwnd = (void*) hwnd;
		window->deviceContext = (void*) GetDC(hwnd);
		window->allocator = allocator;
		return window;
	}

	bool WindowUpdate(Window* window) {
		window->input.Clear();
		window->eventsBuffer.Clear();
		window->mousePos.Clear();

		MSG msg;
		bool ok = true;

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				ok = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//	W32WindowSize size = W32_GetWindowSize((HWND) window->hwnd);
		//char buffer[512];
		//sprintf_s(buffer, 512, "h m   %d  %d \n",size.height, window->mousePosY);
		//OutputDebugString(buffer);

		return ok;
	}

	void WindowSwapBuffers(Window* window) {
		SwapBuffers((HDC) window->deviceContext);
	}

	void* WindowHwnd(Window* window) {
		return window->hwnd;
	}

	void* WindowDeviceContext(Window* window) {
		return window->deviceContext;
	}

	const char* WindowGetEvents(Window* window, u32* outSize) {
		*outSize = (u32) window->eventsBuffer.Size();
		return window->eventsBuffer.Data();
	}

	const PlatformMousePos* WindowGetMousePosEvents(Window* window, u32* outCount) {
		*outCount = window->mousePos.Count();
		return window->mousePos.begin();
	}

	const PlatformKeyType* WindowGetInputEvents(Window* window, u32* outCount) {
		*outCount = window->input.Count();
		return window->input.begin();
	}

	void WindowDeinit(Window* window) {
		auto instance = GetModuleHandle(NULL);
		char cName[64];
		int a = GetClassNameA((HWND) window->hwnd, cName, 64);

		BOOL okk = DestroyWindow((HWND) window->hwnd);
		BOOL ok = UnregisterClassA(cName, instance);
		DWORD e = GetLastError();

		IAllocator* allocator = window->allocator;
		window->~Window();
		Deallocate(allocator, window);
	}


	W32WindowSize W32_GetWindowSize(HWND window) {
		W32WindowSize result;

		RECT windowRect;
		//GetWindowRect(window, &windowRect); // this includes the dumb header
		GetClientRect(window, &windowRect);
		result.width = windowRect.right - windowRect.left;
		result.height = windowRect.bottom - windowRect.top;
		return result;
	}


	LRESULT CALLBACK W32_WindowCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		LRESULT result = 0;

		void* userData = (void*) GetWindowLongPtrA(hwnd, GWLP_USERDATA);
		Window* window = (Window*) userData;

		if (!window) {
			switch (msg) {
			case WM_NCCREATE:
			{
				CREATESTRUCT* createStruct = (CREATESTRUCT*) lParam;
				void* data = (void*) createStruct->lpCreateParams;
				SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR) data);
				Window* w = (Window*) data;
				SetWindowTextA(hwnd, w->name);
				result = TRUE;
				break;
			}
			/*case WM_NCCALCSIZE:
				result = DefWindowProc(window, msg, wParam, lParam);
				break;*/
			default:
				//Assert(false);
				result = DefWindowProc(hwnd, msg, wParam, lParam); // WM_GETMINMAXINFO, WM_NCDESTROY
				break;
			}
		}
		else {


			switch (msg) {
			case WM_SIZE:
			{
				W32WindowSize size = W32_GetWindowSize(hwnd);
				PlatformEventWindowResize resizeEvent = {PLATFORM_EVENT_WINDOW_RESIZE, size.width, size.height};
				window->eventsBuffer.Append((char*) &resizeEvent, sizeof(resizeEvent));
			}
			break;
			case WM_DESTROY:
			{
				PlatformEventType type = PLATFORM_EVENT_CLOSE;
				window->eventsBuffer.Append((char*) &type, sizeof(type));
			}
			break;
			case WM_CLOSE:
			{
				PlatformEventType type = PLATFORM_EVENT_CLOSE;
				window->eventsBuffer.Append((char*) &type, sizeof(type));
			}
			case WM_ACTIVATEAPP:
				break;
			case WM_MOUSEMOVE:
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				window->mousePos.Push({x, y});
				break;
			}
			case WM_LBUTTONDOWN:
				//window->clickDown = true;
				break;
			case WM_LBUTTONUP:
			//	window->clickUp = true;
				break;
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
				u32 vkCode = (u32) wParam;
				bool wasDown = (lParam & (1 << 30)) != 0;
				bool isDown = (lParam & (1 << 31)) == 0;


#define VK_CASE(code, platformCode) case code: window->input.Push(platformCode); break;
				if (isDown) {
					switch (vkCode) {
						VK_CASE(VK_LEFT, PLATFORM_KEY_LEFT);
						VK_CASE(VK_RIGHT, PLATFORM_KEY_RIGHT);
						VK_CASE(VK_UP, PLATFORM_KEY_UP);
						VK_CASE(VK_DOWN, PLATFORM_KEY_DOWN);
						VK_CASE(VK_DELETE, PLATFORM_KEY_DELETE);
						VK_CASE(VK_SHIFT, PLATFORM_KEY_SHIFT);
						VK_CASE(VK_MENU, PLATFORM_KEY_ALT);
						VK_CASE(VK_CONTROL, PLATFORM_KEY_CTRL);
						VK_CASE(VK_F1, PLATFORM_KEY_F1);
					default:
						break;
					}
				}
#undef VK_CASE

				break;
			}
			case WM_CHAR:
			{
				u32 charCode = (u32) wParam;
				bool wasDown = (lParam & (1 << 30)) != 0;
				bool isDown = (lParam & (1 << 31)) == 0;
				if (isDown && ((charCode >= 0x20 && charCode < 0x80) || charCode == 0x8 || charCode == 0x9 || charCode == 0xD)) {
					window->input.Push((PlatformKeyType) charCode);
				}
				break;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT paint;
				BeginPaint(hwnd, &paint);
				EndPaint(hwnd, &paint);
				break;
			}
			default:
				result = DefWindowProc(hwnd, msg, wParam, lParam);
				break;
			}
		}


		return result;
	}

}


//bool RendererInit(Renderer* renderer, void* whdc) {
//	static_assert(sizeof(HDC) == sizeof(void*), "Size missmatch");
//	static_assert(sizeof(HGLRC) == sizeof(void*), "Size missmatch");
//
//	HDC hdc = (HDC) whdc;
//
//	PIXELFORMATDESCRIPTOR pfd;
//	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
//	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
//	pfd.nVersion = 1;
//	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
//	pfd.iPixelType = PFD_TYPE_RGBA;
//	pfd.cColorBits = 32;
//	pfd.cDepthBits = 32;
//	pfd.iLayerType = PFD_MAIN_PLANE;
//
//	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
//
//	if (nPixelFormat == 0)
//		return false;
//
//	BOOL bResult = SetPixelFormat(hdc, nPixelFormat, &pfd);
//
//	if (!bResult)
//		return false;
//
//	HGLRC tempContext = wglCreateContext(hdc);
//	wglMakeCurrent(hdc, tempContext);
//
//	GLenum err = glewInit();
//	if (GLEW_OK != err)
//		return false;
//
//	int attribs[] =
//	{
//		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
//		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
//		WGL_CONTEXT_FLAGS_ARB, 0,
//		0
//	};
//
//	HGLRC rdc;
//
//	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
//		rdc = wglCreateContextAttribsARB(hdc, 0, attribs);
//		wglMakeCurrent(NULL, NULL);
//		wglDeleteContext(tempContext);
//		wglMakeCurrent(hdc, rdc);
//	}
//	else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
//		rdc = tempContext;
//	}
//
//	//Checking GL version
//	const GLubyte *GLVersionString = glGetString(GL_VERSION);
//
//	//Or better yet, use the GL3 way to get the version number
//	int OpenGLVersion[2];
//	glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
//	glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
//
//	if (!rdc)
//		return false;
//
//	FontShaderMaterial fontShaderMaterial;
//	if (!Renderer_FontShaderMaterialInit(&fontShaderMaterial))
//		return false;
//
//	BoxShaderMaterial boxShaderMaterial;
//	if (!Renderer_BoxShaderMaterialInit(&boxShaderMaterial))
//		return false;
//
//
//	renderer->renderDeviceContext = rdc;
//	renderer->deviceContext = hdc;
//	renderer->fontMaterial = fontShaderMaterial;
//	renderer->boxMaterial = boxShaderMaterial;
//
//	RendererResize(renderer, 1024, 768);
//
//	return true;
//}

