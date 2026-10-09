// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"
#include "Engine/Adapter/Window/Win32/PointerType.hpp"
#include "Engine/Adapter/Window/Win32/WndProc.hpp"

#include "Engine/API/Window/Window.hpp"

namespace Trident {
	static constexpr LPCWSTR lpszWndClassExName = L"EngineWindowClass";
	static WindowClass* windowClass = nullptr;
	static uint32 windowCount = 0;

	static void registerWindowClass() {
		ATOM result;

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

		result = RegisterClassEx(&wndClass);

		WIN32_ASSERT(result != 0);

		windowClass = new WindowClass();
		windowClass->className = wndClass.lpszClassName;
		windowClass->hCursor = LoadCursor(nullptr, IDC_ARROW);

		WIN32_ASSERT(windowClass->hCursor != nullptr);
	}

	static void unregisterWindowClass() {
		BOOL result;

		const HINSTANCE hInstance = GetInstance();

		result = DestroyCursor(windowClass->hCursor);

		WIN32_ASSERT(result != 0);

		result = UnregisterClass(windowClass->className, hInstance);

		WIN32_ASSERT(result != 0);

		delete windowClass;

		windowClass = nullptr;
	}

	Window::Window(const WindowDescriptor& descriptor) {
		BOOL result;

		if (windowClass == nullptr) {
			registerWindowClass();
		}

		RECT rect{};
		rect.left = 0;
		rect.top = 0;
		rect.right = descriptor.extent.x;
		rect.bottom = descriptor.extent.y;

		constexpr DWORD dwStyle = WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
		constexpr BOOL bMenu = FALSE;
		constexpr DWORD dwExStyle = WS_EX_APPWINDOW;

		result = AdjustWindowRectEx(&rect, dwStyle, bMenu, dwExStyle);

		WIN32_ASSERT(result != 0);

		const LPCWSTR lpWindowName = ConvertUTF8ToUTF16(&descriptor.name[0]);
		constexpr sint32 X = CW_USEDEFAULT;
		constexpr sint32 Y = CW_USEDEFAULT;
		const sint32 nWidth = rect.right - rect.left;
		const sint32 nHeight = rect.bottom - rect.top;
		const HINSTANCE hInstance = GetInstance();

		data = new Win32WindowData();

		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		// The event loop starts automatically after creating the window.
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

		windowData->extent = descriptor.extent;
		windowData->maximized = descriptor.maximized;
		windowData->keyDownEventListener = descriptor.keyDownEventListener;
		windowData->keyUpEventListener = descriptor.keyUpEventListener;
		windowData->mouseDownEventListener = descriptor.mouseDownEventListener;
		windowData->mouseMoveEventListener = descriptor.mouseMoveEventListener;
		windowData->mouseUpEventListener = descriptor.mouseUpEventListener;
		windowData->mouseWheelEventListener = descriptor.mouseWheelEventListener;
		windowData->windowCloseEventListener = descriptor.windowCloseEventListener;
		windowData->windowResizeEventListener = descriptor.windowResizeEventListener;

		windowData->keyMap = sequence<bool>(static_cast<uint32>(KeyCode::count));

		result = SetProp(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING, windowClass);

		WIN32_ASSERT(result != 0);

		result = SetProp(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING, windowData);

		WIN32_ASSERT(result != 0);

		windowCount += 1;
	}

	Window::~Window() {
		sint32 releaseDeviceContextResult;
		HANDLE removePropertyResult;
		BOOL destroyWindowResult;

		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		releaseDeviceContextResult = ReleaseDC(windowData->hWnd, windowData->hDC);

		WIN32_ASSERT(releaseDeviceContextResult != 0);

		removePropertyResult = RemoveProp(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING);

		WIN32_ASSERT(removePropertyResult != nullptr);

		removePropertyResult = RemoveProp(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING);

		WIN32_ASSERT(removePropertyResult != nullptr);

		destroyWindowResult = DestroyWindow(windowData->hWnd);

		WIN32_ASSERT(destroyWindowResult != 0);

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

	Extent2D Window::getExtent() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.extent;
	}

	uint32 Window::getDpi() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);
		const uint32 dpi = GetDpiForWindow(windowData.hWnd);

		WIN32_ASSERT(dpi != 0);

		return dpi;
	}

	bool Window::isKeyDown(KeyCode keyCode) const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.keyMap[static_cast<uint32>(keyCode)];
	}

	bool Window::isMaximized() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.maximized;
	}

	bool Window::isMinimized() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.minimized;
	}

	vec2<sint32> Window::getMousePosition() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.cursor;
	}

	bool Window::isPointerLocked() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		return windowData.pointerLocked;
	}

	void Window::show() const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		INT nCmdShow = SW_NORMAL;

		if (windowData->maximized) {
			nCmdShow = SW_MAXIMIZE;
		}

		ShowWindow(windowData->hWnd, nCmdShow);
	}

	void Window::lockPointer() const {
		BOOL result;
		RECT rect;

		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		result = GetClientRect(windowData->hWnd, &rect);

		WIN32_ASSERT(result != 0);

		result = ClientToScreen(windowData->hWnd, pointer_cast<LPPOINT>(&rect.left));

		WIN32_ASSERT(result != 0);

		result = ClientToScreen(windowData->hWnd, pointer_cast<LPPOINT>(&rect.right));

		WIN32_ASSERT(result != 0);

		result = ClipCursor(&rect);

		WIN32_ASSERT(result != 0);

		ShowCursor(false);

		windowData->virtualCursorPositionX = 0;
		windowData->virtualCursorPositionY = 0;
		windowData->pointerLocked = true;
	}

	void Window::unlockPointer() const {
		BOOL result;

		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);

		result = ClipCursor(nullptr);

		WIN32_ASSERT(result != 0);

		ShowCursor(true);

		windowData->pointerLocked = false;
	}

	void Window::processMessages() const {
		Win32WindowData* windowData = static_cast<Win32WindowData*>(data);
		MSG msg;
		BOOL result;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// TODO: Test this path.
				DEBUG_ASSERT(false);

				WindowCloseEvent windowCloseEvent;

				windowData->windowCloseEventListener(windowCloseEvent);

				continue;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// TODO(window): Verify that the condition is correct.
		// windowData->cursorX != windowData->extent.x / 2 || windowData->cursorY != windowData->sizeY / 2
		if (windowData->pointerLocked) {
			windowData->cursor = vec2<sint32>(windowData->extent) / 2;

			POINT point{};
			point.x = windowData->cursor.x;
			point.y = windowData->cursor.y;

			result = ClientToScreen(windowData->hWnd, &point);

			WIN32_ASSERT(result != 0);

			result = SetCursorPos(point.x, point.y);

			WIN32_ASSERT(result != 0);
		}
	}

	void Window::waitMessage() const {
		BOOL result;

		result = WaitMessage();

		WIN32_ASSERT(result != 0);
	}

	void Window::updatePointerType(PointerType pointerType) const {
		const LPWSTR lpCursorName = getWin32CursorName(pointerType);

		windowClass->hCursor = LoadCursor(nullptr, lpCursorName);

		WIN32_ASSERT(windowClass->hCursor != nullptr);
	}
}