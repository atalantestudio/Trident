#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class PrimitiveTopology : uint32 {
		POINT_LIST = 0,
		LINE_LIST = 1,
		LINE_STRIP = 2,
		TRIANGLE_LIST = 3,
		TRIANGLE_STRIP = 4,
	};
}