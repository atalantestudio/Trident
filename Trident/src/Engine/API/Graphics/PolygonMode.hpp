// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PolygonMode : uint8 {
		/// Specifies that polygon vertices are drawn as points.
		POINT,

		/// Specifies that polygon edges are drawn as line segments.
		LINE,

		/// Specifies that polygons are rendered filled.
		FILL,

		count,
	};
}