#include "pch.hpp"

#include "Backend/Win32/base.hpp"

namespace ProjectA {
	const char8* Platform::windowName = nullptr;

	void Platform::initialize(const char8* windowName) {
		Platform::windowName = windowName;
	}

	uint32 Platform::getCurrentProcessId() {
		const DWORD pid = GetCurrentProcessId();

		return pid;
	}

	PlatformLibrary Platform::openLibrary(std::string_view libraryName) {
		return PlatformLibrary(libraryName);
	}

	void Platform::fatalExit(uint32 code, const std::string& message) {
		const LPCWSTR titleString = ConvertUTF8ToUTF16(windowName);
		const LPCWSTR messageString = ConvertUTF8ToUTF16(message.data());

		WIN32_ASSERT(MessageBoxW(nullptr, messageString, titleString, MB_OK | MB_ICONERROR) != 0);

		free((void*) titleString);
		free((void*) messageString);

		ExitProcess(code);
	}

	void Platform::waitForInput() {
		std::cin.get();
	}
}