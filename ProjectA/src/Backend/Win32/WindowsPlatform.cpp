#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/Platform.hpp"

namespace ProjectA {
	std::string Platform::projectName;

	void Platform::setProjectName(const std::string& projectName) {
		Platform::projectName = projectName;
	}

	float64 Platform::getTime() {
		static LARGE_INTEGER frequency;
		static const BOOL result = QueryPerformanceFrequency(&frequency);

		LARGE_INTEGER performanceCount;

		WIN32_ASSERT(QueryPerformanceCounter(&performanceCount));

		return performanceCount.QuadPart / static_cast<float64>(frequency.QuadPart);
	}

	void Platform::fatalExit(uint32 code, const std::string& message) {
		const LPCWSTR titleString = ConvertUTF8ToUTF16(Platform::projectName.data());
		const LPCWSTR messageString = ConvertUTF8ToUTF16(message.data());

		WIN32_ASSERT(MessageBox(nullptr, messageString, titleString, MB_OK | MB_ICONERROR) != 0);

		free((void*) titleString);
		free((void*) messageString);

		ExitProcess(code);
	}
}