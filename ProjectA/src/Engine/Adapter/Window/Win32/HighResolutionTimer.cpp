// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

#include "Engine/API/Timer/HighResolutionTimer.hpp"

namespace ProjectA {
	HighResolutionTimer::HighResolutionTimer() {
		LARGE_INTEGER performanceCount;

		WIN32_ASSERT(QueryPerformanceCounter(&performanceCount) != 0);

		startTime = performanceCount.QuadPart;

		LARGE_INTEGER frequency;

		WIN32_ASSERT(QueryPerformanceFrequency(&frequency) != 0);

		timeFrequency = frequency.QuadPart;
	}

	float64 HighResolutionTimer::getCurrentTime() const {
		LARGE_INTEGER currentTime;

		WIN32_ASSERT(QueryPerformanceCounter(&currentTime) != 0);

		return static_cast<float64>(currentTime.QuadPart - startTime) / timeFrequency;
	}
}