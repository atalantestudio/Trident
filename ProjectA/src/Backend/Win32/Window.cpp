#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/Window/Window.hpp"

namespace ProjectA {
	static void Show(const Window& window) {
		constexpr sint32 nCmdShow = SW_NORMAL;

		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(window.data);

		ShowWindow(windowData->hWnd, nCmdShow);
	}

	Window::Window(const WindowClass& windowClass, const WindowDescriptor& descriptor) {
		const HINSTANCE hInstance = GetInstance();
		const Win32WindowClassData* windowClassData = static_cast<const Win32WindowClassData*>(windowClass.data);

		Win32WindowData* windowData = new Win32WindowData();

		windowData->hWnd = CreateWindowExW(
			0,
			windowClassData->className,
			ConvertUTF8ToUTF16(descriptor.name),
			WS_OVERLAPPEDWINDOW,
			descriptor.x,
			descriptor.y,
			descriptor.width,
			descriptor.height,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		ENGINE_ASSERT(windowData->hWnd != nullptr, "Failed to create Win32 window.");

		windowData->hDC = GetDC(windowData->hWnd);

		ENGINE_ASSERT(windowData->hDC != nullptr, "Failed to get Win32 device context.");

		data = windowData;

		BOOL result;

		result = SetPropW(windowData->hWnd, WINDOW_CLASS_DATA_PROPERTY_STRING, (void*) windowClass.data);

		ENGINE_ASSERT(result, "Failed to set window property.");

		result = SetPropW(windowData->hWnd, WINDOW_DATA_PROPERTY_STRING, windowData);

		ENGINE_ASSERT(result, "Failed to set window data property.");

		Show(*this);
	}

	Window::~Window() {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		{
			const sint32 result = ReleaseDC(windowData->hWnd, windowData->hDC);

			ENGINE_ASSERT(result != 0, "Failed to release Win32 device context.");
		}

		const BOOL result = DestroyWindow(windowData->hWnd);

		ENGINE_ASSERT(result, "Failed to destroy Win32 window.");
	}

	const sint32* Window::getMousePosition() const {
		const Win32WindowData* windowData = static_cast<const Win32WindowData*>(data);

		return &windowData->cursorX;
	}

	void Window::dispatchMessages() const {
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
}