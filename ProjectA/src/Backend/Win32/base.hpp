#pragma once

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
	};

	struct Win32WindowClassData {
		LPCWSTR className;

		// Handle to the cursor within the client area.
		// Defaults to the normal cursor (IDC_ARROW).
		HCURSOR hCursor;
	};

	static constexpr LPCWSTR WINDOW_CLASS_DATA_PROPERTY_STRING = L"WindowClassDataProperty";
	static constexpr LPCWSTR WINDOW_DATA_PROPERTY_STRING = L"WindowDataProperty";

	static HINSTANCE GetInstance() {
		const HINSTANCE hInstance = GetModuleHandleW(nullptr);

		ENGINE_ASSERT(hInstance != nullptr, "Failed to get the instance of the window procedure.");

		return hInstance;
	}

	// Converts an UTF-8 string to UTF-16.
	// The string is assumed to be null-terminated.
	static LPCWSTR ConvertUTF8ToUTF16(LPCCH string) {
		const sint32 characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);

		ENGINE_ASSERT(characterCount > 0, "Failed to get character count of UTF-8 string.");

		LPWSTR convertedString = static_cast<LPWSTR>(calloc(characterCount, sizeof(WCHAR)));

		if (MultiByteToWideChar(CP_UTF8, 0, string, -1, convertedString, characterCount) == 0) {
			free(convertedString);

			ENGINE_ASSERT(false, "Failed to convert UTF-8 string to UTF-16.");

			return nullptr;
		}

		return convertedString;
	}
}