#pragma once

namespace ProjectA {
	// TODO: This struct is very similar to SDFGlyph. Maybe merge them?
	struct TrueTypeGlyph {
		// Top-left coordinate.
		svec2<float32> minCoordinate;

		// Bottom-right coordinate.
		svec2<float32> maxCoordinate;

		// In FUnits.
		uint16 width;

		// In FUnits.
		uint16 advanceWidth;

		// In FUnits.
		sint16 leftSideBearing;

		// True for spaces, tabs, etc.
		bool invisible;
	};
}