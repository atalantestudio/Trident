// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/EventListener.hpp"
#include "Engine/API/Event/Keyboard/KeyDownEvent.hpp"
#include "Engine/API/Event/Keyboard/KeyUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

#if !defined(WIN32_LEAN_AND_MEAN)
	// Exclude rarely-used stuff from Windows headers.
	#define WIN32_LEAN_AND_MEAN
#endif

namespace Trident {
	struct Win32WindowData {
		// Window handle.
		HWND hWnd;

		// Device context handle.
		HDC hDC;

		// TODO: Use vec2<sint32> (no SIMD).
		sint32 positionX;
		sint32 positionY;

		// TODO: Use vec2<sint32> (no SIMD).
		Extent2D extent;

		bool minimized;
		bool maximized;
		bool pointerLocked;
		vec2<sint32> cursor;

		// TODO: Check if that is really needed.
		// TODO: Use vec2<sint32> (no SIMD).
		sint32 virtualCursorPositionX = 0;
		sint32 virtualCursorPositionY = 0;

		EventListener<KeyDownEvent> keyDownEventListener;
		EventListener<KeyUpEvent> keyUpEventListener;
		EventListener<MouseDownEvent> mouseDownEventListener;
		EventListener<MouseMoveEvent> mouseMoveEventListener;
		EventListener<MouseUpEvent> mouseUpEventListener;
		EventListener<MouseWheelEvent> mouseWheelEventListener;
		EventListener<WindowResizeEvent> windowResizeEventListener;
		EventListener<WindowCloseEvent> windowCloseEventListener;

		sequence<bool> keyMap;
	};

	struct WindowClass {
		LPCWSTR className;

		// Handle to the cursor within the client area.
		// Defaults to the normal cursor (IDC_ARROW).
		HCURSOR hCursor;
	};

	static constexpr LPCWSTR WINDOW_CLASS_DATA_PROPERTY_STRING = L"WindowClassDataProperty";
	static constexpr LPCWSTR WINDOW_DATA_PROPERTY_STRING = L"WindowDataProperty";

	void PrintLastError();

	#if defined(BUILD_MODE_DEBUG)
		#define WIN32_ASSERT(condition) { \
			if (!(condition)) { \
				PrintLastError(); \
				BREAKPOINT(); \
			} \
		}
	#else
		#define WIN32_ASSERT
	#endif

	inline HINSTANCE GetInstance() {
		const HINSTANCE hInstance = GetModuleHandle(nullptr);

		WIN32_ASSERT(hInstance != nullptr);

		return hInstance;
	}

	// Converts an UTF-8 string to UTF-16.
	// The string is assumed to be null-terminated.
	inline LPWSTR ConvertUTF8ToUTF16(LPCCH lpMultiByteStr) {
		const sint32 cchWideChar = MultiByteToWideChar(CP_UTF8, 0, lpMultiByteStr, -1, nullptr, 0);

		WIN32_ASSERT(cchWideChar > 0);

		LPWSTR lpWideCharStr = static_cast<LPWSTR>(calloc(cchWideChar, sizeof(WCHAR)));

		if (MultiByteToWideChar(CP_UTF8, 0, lpMultiByteStr, -1, lpWideCharStr, cchWideChar) == 0) {
			free(lpWideCharStr);

			WIN32_ASSERT(false);

			return nullptr;
		}

		return lpWideCharStr;
	}

	// Converts an UTF-16 string to UTF-8.
	// The string is assumed to be null-terminated.
	inline LPSTR ConvertUTF16ToUTF8(LPCWCH lpWideCharStr) {
		const sint32 cbMultiByte = WideCharToMultiByte(CP_UTF8, 0, lpWideCharStr, -1, nullptr, 0, nullptr, nullptr);

		WIN32_ASSERT(cbMultiByte > 0);

		LPSTR lpMultiByteStr = static_cast<LPSTR>(calloc(cbMultiByte, sizeof(CHAR)));

		if (WideCharToMultiByte(CP_UTF8, 0, lpWideCharStr, -1, lpMultiByteStr, cbMultiByte, nullptr, nullptr) == 0) {
			free(lpMultiByteStr);

			WIN32_ASSERT(false);

			return nullptr;
		}

		return lpMultiByteStr;
	}

	inline void PrintLastError() {
		const DWORD dwMessageId = GetLastError();
		constexpr DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

		LPWSTR buffer = nullptr;

		const DWORD nSize = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			dwMessageId,
			dwLanguageId,
			(LPWSTR) &buffer,
			0,
			nullptr
		);

		if (nSize == 0) {
			ENGINE_ASSERT(false, "Failed to format Win32 error message.");
			BREAKPOINT();

			LocalFree(buffer);
			ExitProcess(dwMessageId);

			return;
		}

		const LPCSTR message = ConvertUTF16ToUTF8(buffer);

		ENGINE_ASSERT(false, message);

		LocalFree(buffer);
		LocalFree((void*) &message);
		ExitProcess(dwMessageId);
	}
}