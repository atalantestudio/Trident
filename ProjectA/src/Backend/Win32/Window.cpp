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

		// ENGINE_DEBUG("Registered window class.");
	}

	static void unregisterWindowClass() {
		const HINSTANCE hInstance = GetInstance();

		WIN32_ASSERT(DestroyCursor(windowClass->hCursor));
		WIN32_ASSERT(UnregisterClassW(windowClass->className, hInstance));

		delete windowClass;

		windowClass = nullptr;

		// ENGINE_DEBUG("Unregistered window class.");
	}

	Window::Window(const WindowDescriptor& descriptor) {
		if (windowClass == nullptr) {
			registerWindowClass();
		}

		RECT rect{};
		rect.left = 0;
		rect.top = 0;
		rect.right = descriptor.size.width;
		rect.bottom = descriptor.size.height;

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

		data = new Win32WindowData();

		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		// The event loop starts automatically after creating the window.
		windowData->hWnd = CreateWindowExW(
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

		windowData->sizeX = descriptor.size.width;
		windowData->sizeY = descriptor.size.height;

		windowData->mouseDownEventListener = descriptor.mouseDownEventListener;
		windowData->mouseMoveEventListener = descriptor.mouseMoveEventListener;
		windowData->mouseUpEventListener = descriptor.mouseUpEventListener;
		windowData->mouseWheelEventListener = descriptor.mouseWheelEventListener;
		windowData->windowCloseEventListener = descriptor.windowCloseEventListener;
		windowData->windowResizeEventListener = descriptor.windowResizeEventListener;

		WIN32_ASSERT(SetPropW(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING, windowClass));
		WIN32_ASSERT(SetPropW(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING, windowData));

		windowCount += 1;
	}

	Window::~Window() {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		WIN32_ASSERT(ReleaseDC(windowData->hWnd, windowData->hDC) != 0);

		WIN32_ASSERT(RemovePropW(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING) != nullptr);
		WIN32_ASSERT(RemovePropW(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING) != nullptr);

		WIN32_ASSERT(DestroyWindow(windowData->hWnd));

		if (windowCount == 1) {
			unregisterWindowClass();

			windowCount = 0;
		}
		else {
			windowCount -= 1;
		}

		delete windowData;

		data = nullptr;
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

	void Window::show() const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		// The event loop starts after showing the window.
		ShowWindow(windowData->hWnd, SW_NORMAL);
	}

	void Window::processMessages() const {
		MSG msg;

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// TODO: Test this path.
				DEBUG_ASSERT(false);

				WindowCloseEvent windowCloseEvent;

				const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);
				windowData->windowCloseEventListener(windowCloseEvent);

				continue;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void Window::waitMessage() const {
		WIN32_ASSERT(WaitMessage() != 0);
	}
}