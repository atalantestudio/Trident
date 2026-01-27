// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PolygonMode : uint8 {
		// Specifies that polygon vertices are drawn as points.
		POINT = 0,

		// Specifies that polygon edges are drawn as line segments.
		LINE = 1,

		// Specifies that polygons are rendered filled.
		FILL = 2,

		count,
	};
}