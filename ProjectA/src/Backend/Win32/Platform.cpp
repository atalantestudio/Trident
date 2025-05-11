#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/Platform.hpp"

namespace ProjectA {
	const char* Platform::windowName = nullptr;

	const char* Platform::getWindowName() {
		return windowName;
	}

	void Platform::setWindowName(const char* windowName) {
		Platform::windowName = windowName;
	}

	uint32 Platform::getCurrentProcessId() {
		const DWORD pid = GetCurrentProcessId();

		return pid;
	}

	void Platform::fatalExit(uint32 code, const std::string& message) {
		const LPCWSTR titleString = ConvertUTF8ToUTF16(windowName);
		const LPCWSTR messageString = ConvertUTF8ToUTF16(message.data());

		WIN32_ASSERT(MessageBox(nullptr, messageString, titleString, MB_OK | MB_ICONERROR) != 0);

		free((void*) titleString);
		free((void*) messageString);

		ExitProcess(code);
	}

	void Platform::waitForInput() {
		std::cin.get();
	}
}