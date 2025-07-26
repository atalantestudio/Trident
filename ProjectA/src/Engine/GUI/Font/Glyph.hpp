#pragma once

namespace ProjectA {
	// A glyph from a bitmap or TrueType font.
	struct Glyph {
		// Top-left coordinate on the glyph map.
		svec2<float32> minCoordinate;

		// Bottom-right coordinate on the glyph map.
		svec2<float32> maxCoordinate;

		// (TTF) In FUnits.
		uint16 width;

		// (TTF) In FUnits.
		uint16 advanceWidth;

		// (TTF) In FUnits.
		sint16 leftSideBearing;

		// Spaces, tabs, etc.
		bool whitespace;
	};
}