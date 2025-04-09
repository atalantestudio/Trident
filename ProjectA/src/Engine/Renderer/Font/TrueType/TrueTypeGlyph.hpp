#pragma once

namespace ProjectA {
	struct TrueTypeGlyph {
		// Top-left coordinate.
		float32 minCoordinateX;
		float32 minCoordinateY;

		// Bottom-right coordinate.
		float32 maxCoordinateX;
		float32 maxCoordinateY;

		// In FUnits.
		uint16 width;

		// In FUnits.
		uint16 advanceWidth;

		// In FUnits.
		sint16 leftSideBearing;

		bool invisible;
	};
}