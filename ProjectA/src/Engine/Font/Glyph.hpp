#pragma once

namespace ProjectA {
	// A glyph from a bitmap or TrueType font.
	struct Glyph {
		// Top-left coordinate on the glyph map.
		svec2<float32> minCoordinate;

		// Bottom-right coordinate on the glyph map.
		svec2<float32> maxCoordinate;

		// In TTF units.
		uint16 width;

		// In TTF units.
		uint16 advanceWidth;

		// In TTF units.
		uint16 height;

		// In TTF units.
		sint16 leftSideBearing;

		// In TTF units.
		sint16 topSideBearing;

		// Spaces, tabs, etc.
		bool whitespace;
	};
}