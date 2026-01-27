// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

#include "Engine/API/Timer/HighResolutionTimer.hpp"

namespace Trident {
	HighResolutionTimer::HighResolutionTimer() {
		BOOL result;
		LARGE_INTEGER performanceCount;

		result = QueryPerformanceCounter(&performanceCount);

		WIN32_ASSERT(result != 0);

		startTime = performanceCount.QuadPart;

		LARGE_INTEGER frequency;

		result = QueryPerformanceFrequency(&frequency);

		WIN32_ASSERT(result != 0);

		timeFrequency = frequency.QuadPart;
	}

	float64 HighResolutionTimer::getCurrentTime() const {
		LARGE_INTEGER currentTime;
		BOOL result;

		result = QueryPerformanceCounter(&currentTime);

		WIN32_ASSERT(result != 0);

		return static_cast<float64>(currentTime.QuadPart - startTime) / timeFrequency;
	}
}