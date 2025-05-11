#include "pch.hpp"

#include "Backend/Win32/base.hpp"
#include "Backend/Win32/WndProc.hpp"

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	static constexpr LPCWSTR lpszWndClassExName = L"EngineWindowClass";
	static WindowClass* windowClass = nullptr;
	static uint32 windowCount = 0;

	static void registerWindowClass() {
		const HINSTANCE hInstance = GetInstance();

		WNDCLASSEX wndClass{};
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = nullptr;
		wndClass.hCursor = nullptr;
		wndClass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = lpszWndClassExName;
		wndClass.hIconSm = nullptr;

		WIN32_ASSERT(RegisterClassExW(&wndClass) != 0);

		windowClass = new WindowClass();
		windowClass->className = wndClass.lpszClassName;
		windowClass->hCursor = LoadCursorW(nullptr, IDC_ARROW);

		ENGINE_DEBUG("Registered window class.");
	}

	static void unregisterWindowClass() {
		const HINSTANCE hInstance = GetInstance();

		WIN32_ASSERT(DestroyCursor(windowClass->hCursor));
		WIN32_ASSERT(UnregisterClassW(windowClass->className, hInstance));

		delete windowClass;

		windowClass = nullptr;

		ENGINE_DEBUG("Unregistered window class.");
	}

	Window::Window(const WindowDescriptor& descriptor) {
		if (windowClass == nullptr) {
			registerWindowClass();
		}

		RECT rect{};
		rect.left = 0;
		rect.top = 0;
		rect.right = descriptor.width;
		rect.bottom = descriptor.height;

		constexpr DWORD dwStyle = WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
		constexpr BOOL bMenu = FALSE;
		constexpr DWORD dwExStyle = WS_EX_APPWINDOW;

		WIN32_ASSERT(AdjustWindowRectEx(&rect, dwStyle, bMenu, dwExStyle) != 0);

		const LPCWSTR lpWindowName = ConvertUTF8ToUTF16(descriptor.name.data());
		constexpr sint32 X = CW_USEDEFAULT;
		constexpr sint32 Y = CW_USEDEFAULT;
		const sint32 nWidth = rect.right - rect.left;
		const sint32 nHeight = rect.bottom - rect.top;
		const HINSTANCE hInstance = GetInstance();

		Win32WindowData* windowData = new Win32WindowData();

		windowData->hWnd = CreateWindowEx(
			dwExStyle,
			windowClass->className,
			lpWindowName,
			dwStyle,
			X,
			Y,
			nWidth,
			nHeight,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		free((void*) lpWindowName);

		WIN32_ASSERT(windowData->hWnd != nullptr);

		windowData->hDC = GetDC(windowData->hWnd);

		WIN32_ASSERT(windowData->hDC != nullptr);

		WIN32_ASSERT(SetProp(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING, windowClass));
		WIN32_ASSERT(SetProp(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING, windowData));

		data = windowData;
		windowCount += 1;

		constexpr sint32 nCmdShow = SW_NORMAL;

		ShowWindow(windowData->hWnd, nCmdShow);

		PIXELFORMATDESCRIPTOR pfd{};
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cRedBits = 0;
		pfd.cRedShift = 0;
		pfd.cGreenBits = 0;
		pfd.cGreenShift = 0;
		pfd.cBlueBits = 0;
		pfd.cBlueShift = 0;
		pfd.cAlphaBits = 0;
		pfd.cAlphaShift = 0;
		pfd.cAccumBits = 0;
		pfd.cAccumRedBits = 0;
		pfd.cAccumGreenBits = 0;
		pfd.cAccumBlueBits = 0;
		pfd.cAccumAlphaBits = 0;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.cAuxBuffers = 0;
		pfd.iLayerType = 0;
		pfd.bReserved = 0;
		pfd.dwLayerMask = 0;
		pfd.dwVisibleMask = 0;
		pfd.dwDamageMask = 0;

		const sint32 format = ChoosePixelFormat(windowData->hDC, &pfd);

		WIN32_ASSERT(format != 0);

		WIN32_ASSERT(SetPixelFormat(windowData->hDC, format, &pfd) == TRUE);

		const HGLRC hGLRC = wglCreateContext(windowData->hDC);

		WIN32_ASSERT(hGLRC != nullptr);

		WIN32_ASSERT(wglMakeCurrent(windowData->hDC, hGLRC) == TRUE);

		windowData->wglCreateContextAttribsARB = static_cast<const PFNWGLCREATECONTEXTATTRIBSARBPROC>(static_cast<void*>(wglGetProcAddress("wglCreateContextAttribsARB")));

		WIN32_ASSERT(windowData->wglCreateContextAttribsARB != nullptr);

		WIN32_ASSERT(wglMakeCurrent(windowData->hDC, nullptr) == TRUE);

		WIN32_ASSERT(wglDeleteContext(hGLRC) == TRUE);
	}

	Window::~Window() {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		WIN32_ASSERT(ReleaseDC(windowData->hWnd, windowData->hDC) != 0);

		WIN32_ASSERT(RemoveProp(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING) != nullptr);
		WIN32_ASSERT(RemoveProp(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING) != nullptr);

		WIN32_ASSERT(DestroyWindow(windowData->hWnd));

		delete windowData;

		data = nullptr;
		windowCount -= 1;

		if (windowCount == 0) {
			unregisterWindowClass();
		}
	}

	const sint32* Window::getPosition() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return &windowData.positionX;
	}

	const sint32* Window::getSize() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return &windowData.sizeX;
	}

	bool Window::isMaximized() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.maximized;
	}

	bool Window::isMinimized() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.minimized;
	}

	const sint32* Window::getMousePosition() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return &windowData.cursorX;
	}

	void Window::processMessages() const {
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// TODO: Handle WM_QUIT message.

				continue;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	template<>
	void Window::setEventListener(EventListener<MouseMoveEvent> eventListener) const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		windowData->mouseMoveEventListener = eventListener;
	}

	template<>
	void Window::setEventListener(EventListener<MouseWheelEvent> eventListener) const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		windowData->mouseWheelEventListener = eventListener;
	}

	template<>
	void Window::setEventListener(EventListener<WindowResizeEvent> eventListener) const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		windowData->windowResizeEventListener = eventListener;
	}

	template<>
	void Window::setEventListener(EventListener<WindowCloseEvent> eventListener) const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		windowData->windowCloseEventListener = eventListener;
	}
}