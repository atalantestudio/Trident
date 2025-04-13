#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	static scope<WindowClass> windowClass;

	static void Show(const Window& window) {
		constexpr sint32 nCmdShow = SW_NORMAL;

		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);

		ShowWindow(windowData->hWnd, nCmdShow);
	}

	Window::Window(const WindowDescriptor& descriptor) {
		if (windowClass == nullptr) {
			WindowClassDescriptor windowClassDescriptor{};
			windowClassDescriptor.name = "EngineWindowClass";

			windowClass = createScope<WindowClass>(windowClassDescriptor);
		}

		const HINSTANCE hInstance = GetInstance();
		const Win32WindowClassData* windowClassData = static_cast<const Win32WindowClassData*>(windowClass->data);

		RECT rect{};
		rect.left = descriptor.x;
		rect.top = descriptor.y;
		rect.right = descriptor.x + descriptor.width;
		rect.bottom = descriptor.y + descriptor.height;

		WIN32_ASSERT(AdjustWindowRectEx(&rect, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU, TRUE, WS_EX_APPWINDOW | WS_EX_TOPMOST));

		Win32WindowData* windowData = new Win32WindowData();

		const LPCWSTR lpWindowName = ConvertUTF8ToUTF16(descriptor.name);

		windowData->hWnd = CreateWindowExW(
			0,
			windowClassData->className,
			lpWindowName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		free((void*) lpWindowName);

		WIN32_ASSERT(windowData->hWnd != nullptr);

		windowData->hDC = GetDC(windowData->hWnd);

		WIN32_ASSERT(windowData->hDC != nullptr);

		WIN32_ASSERT(SetPropW(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING, (void*) windowClassData));
		WIN32_ASSERT(SetPropW(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING, windowData));

		data = windowData;

		Show(*this);

		windowData->fatalMbp.cbSize = sizeof(windowData->fatalMbp);
		windowData->fatalMbp.hwndOwner = windowData->hWnd;
		windowData->fatalMbp.hInstance = nullptr;
		windowData->fatalMbp.lpszText = nullptr;
		windowData->fatalMbp.lpszCaption = ENGINE_NAME_L;
		windowData->fatalMbp.dwStyle = MB_OK | MB_ICONERROR;
		windowData->fatalMbp.lpszIcon = nullptr;
		windowData->fatalMbp.dwContextHelpId = 0;
		windowData->fatalMbp.lpfnMsgBoxCallback = nullptr;
		windowData->fatalMbp.dwLanguageId = 0;
	}

	Window::~Window() {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		WIN32_ASSERT(ReleaseDC(windowData->hWnd, windowData->hDC) != 0);

		RemovePropW(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING);
		RemovePropW(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING);

		WIN32_ASSERT(DestroyWindow(windowData->hWnd));

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

	void Window::processMessages() const {
		MSG msg;

		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				// TODO: Handle WM_QUIT message.

				continue;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	/*void Window::swapBuffers() const {
		const Win32WindowData& windowData = *static_cast<const Win32WindowData*>(data);

		// TODO: Use only with OpenGL device context.
		WIN32_ASSERT(SwapBuffers(windowData.hDC));
	}*/

	void Window::showFatalErrorMessage(const std::string& message) const {
		Win32WindowData& windowData = *((Win32WindowData*) data);

		windowData.fatalMbp.lpszText = ConvertUTF8ToUTF16(message.data());

		MessageBoxIndirect(&windowData.fatalMbp);

		free((void*) windowData.fatalMbp.lpszText);

		ExitProcess(2);
	}
}