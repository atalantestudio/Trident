#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class PrimitiveTopology : uint8 {
		POINT_LIST = 0,
		LINE_LIST = 1,
		LINE_STRIP = 2,
		TRIANGLE_LIST = 3,
		TRIANGLE_STRIP = 4,

		// On Vulkan, TRIANGLE_FAN is only supported with an extension.
		// It will be added when the device is updated to automatically enable needed extensions.
	};
}