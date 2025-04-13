#include "pch.hpp"

#include "Backend/Win32/base.hpp"
#include "Backend/Win32/WndProc.hpp"

#include "Engine/API/Event/MouseEvent.hpp"
#include "Engine/API/Window/WindowClass.hpp"

namespace ProjectA {
	WindowClass::WindowClass(const WindowClassDescriptor& descriptor) {
		const HINSTANCE hInstance = GetInstance();

		WNDCLASSEXW wndClass{};
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
		wndClass.lpszClassName = ConvertUTF8ToUTF16(descriptor.name);
		wndClass.hIconSm = nullptr;

		WIN32_ASSERT(RegisterClassExW(&wndClass) != 0);

		Win32WindowClassData* windowClassData = new Win32WindowClassData();
		windowClassData->className = wndClass.lpszClassName;
		windowClassData->hCursor = LoadCursorW(nullptr, IDC_ARROW);

		data = windowClassData;
	}

	WindowClass::~WindowClass() {
		const HINSTANCE hInstance = GetInstance();
		const Win32WindowClassData* windowClassData = static_cast<const Win32WindowClassData*>(data);

		WIN32_ASSERT(DestroyCursor(windowClassData->hCursor));
		WIN32_ASSERT(UnregisterClassW(windowClassData->className, hInstance));

		delete windowClassData;

		data = nullptr;
	}
}