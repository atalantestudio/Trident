#pragma once

namespace ProjectA {
	#define PROJECT_ROOT_DIRECTORY "/"

	// For glyphs with a single contour, checks if the contour is clockwise.
	// If so, inverts it before building the glyph map.
	#define TTF_FIX_INVERTED_CONTOURS
}