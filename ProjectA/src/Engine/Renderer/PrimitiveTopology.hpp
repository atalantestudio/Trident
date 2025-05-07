#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class PrimitiveTopology : uint8 {
		// Specifies a series of separate point primitives.
		POINT_LIST = 0,

		// Specifies a series of separate line primitives.
		LINE_LIST = 1,

		// Specifies a series of connected line primitives with consecutive lines sharing a vertex.
		LINE_STRIP = 2,

		// Specifies a series of separate triangle primitives.
		TRIANGLE_LIST = 3,

		// Specifies a series of connected triangle primitives with consecutive triangles sharing an edge.
		TRIANGLE_STRIP = 4,

		// TODO: TRIANGLE_FAN
		// On Vulkan, TRIANGLE_FAN is only supported with an extension.
		// It will be added when the device is updated to automatically enable needed extensions.
	};
}