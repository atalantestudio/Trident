// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

namespace Trident {
	const char8* Platform::windowName = nullptr;

	void Platform::initialize(const char8* windowName) {
		Platform::windowName = windowName;
	}

	uint32 Platform::getCurrentProcessId() {
		return GetCurrentProcessId();
	}

	PlatformLibrary Platform::openLibrary(view<char8> libraryName) {
		return PlatformLibrary(libraryName);
	}

	void Platform::fatalExit(uint32 code, view<char8> message) {
		sint32 result;

		const LPCWSTR titleString = ConvertUTF8ToUTF16(windowName);
		const LPCWSTR messageString = ConvertUTF8ToUTF16(&message[0]);

		result = MessageBoxW(nullptr, messageString, titleString, MB_OK | MB_ICONERROR);

		WIN32_ASSERT(result != 0);

		free((void*) titleString);
		free((void*) messageString);

		ExitProcess(code);
	}

	void Platform::waitForInput() {
		std::cin.get();
	}
}