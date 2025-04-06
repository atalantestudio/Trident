#pragma once

namespace ProjectA {
	struct TrueTypeGlyph {
		// Top-left coordinate.
		float32 minCoordinateX;
		float32 minCoordinateY;

		// Bottom-right coordinate.
		float32 maxCoordinateX;
		float32 maxCoordinateY;

		// Width in pixels.
		float32 width;

		// In pixels.
		float32 advanceWidth;
		float32 leftSideBearing;

		bool invisible;
	};
}