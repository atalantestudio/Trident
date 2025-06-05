#pragma once

namespace ProjectA {
	enum class PolygonMode : uint8 {
		// Specifies that polygon vertices are drawn as points.
		POINT = 0,

		// Specifies that polygon edges are drawn as line segments.
		LINE = 1,

		// Specifies that polygons are rendered filled.
		FILL = 2,
	};
}