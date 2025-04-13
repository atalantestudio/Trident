#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/Platform.hpp"

namespace ProjectA {
	float64 Platform::getTime() {
		static LARGE_INTEGER frequency;
		static const BOOL result = QueryPerformanceFrequency(&frequency);

		LARGE_INTEGER performanceCount;

		WIN32_ASSERT(QueryPerformanceCounter(&performanceCount));

		return performanceCount.QuadPart / static_cast<float64>(frequency.QuadPart);
	}
}