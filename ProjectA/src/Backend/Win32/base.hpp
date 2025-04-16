#pragma once

#include "Engine/API/Event/Window/WindowCloseEvent.hpp"

namespace ProjectA {
	struct Win32WindowData {
		// Window handle.
		HWND hWnd;

		// Device context handle.
		HDC hDC;

		// TODO: Use vec2<sint32>.
		sint32 positionX;
		sint32 positionY;

		// TODO: Use vec2<sint32>.
		sint32 sizeX;
		sint32 sizeY;

		bool minimized;
		bool maximized;

		// TODO: Use vec2<sint32>.
		sint32 cursorX;
		sint32 cursorY;

		EventListener<WindowCloseEvent> windowCloseEventListener;

		void (*test)();
	};

	struct WindowClass {
		LPCWSTR className;

		// Handle to the cursor within the client area.
		// Defaults to the normal cursor (IDC_ARROW).
		HCURSOR hCursor;
	};

	static constexpr LPCWSTR WINDOW_CLASS_DATA_PROPERTY_STRING = L"WindowClassDataProperty";
	static constexpr LPCWSTR WINDOW_DATA_PROPERTY_STRING = L"WindowDataProperty";

	static void PrintLastError() {
		LPWSTR lpBuffer{};

		const DWORD dwMessageId = GetLastError();
		const DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

		if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, dwMessageId, dwLanguageId, lpBuffer, 0, nullptr) == 0) {
			ENGINE_ASSERT(false, "Failed to format Win32 error message.");

			ExitProcess(GetLastError());

			return;
		}

		ENGINE_ERROR(lpBuffer);

		LocalFree(lpBuffer);
		ExitProcess(dwMessageId);
	}

	#define WIN32_ASSERT(condition) { \
		if (!(condition)) { \
			PrintLastError(); \
			BREAKPOINT(); \
		} \
	}

	static HINSTANCE GetInstance() {
		const HINSTANCE hInstance = GetModuleHandleW(nullptr);

		WIN32_ASSERT(hInstance != nullptr);

		return hInstance;
	}

	// Converts an UTF-8 string to UTF-16.
	// The string is assumed to be null-terminated.
	static LPCWSTR ConvertUTF8ToUTF16(LPCCH string) {
		const sint32 characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);

		WIN32_ASSERT(characterCount > 0);

		LPWSTR convertedString = static_cast<LPWSTR>(calloc(characterCount, sizeof(WCHAR)));

		if (MultiByteToWideChar(CP_UTF8, 0, string, -1, convertedString, characterCount) == 0) {
			free(convertedString);

			WIN32_ASSERT(false);

			return nullptr;
		}

		return convertedString;
	}
}