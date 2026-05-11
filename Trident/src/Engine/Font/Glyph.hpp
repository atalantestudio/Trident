// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// A glyph from a bitmap or TrueType font.
	struct Glyph {
		// Top-left coordinate on the glyph map.
		svec2<float32> minCoordinate;

		// Bottom-right coordinate on the glyph map.
		svec2<float32> maxCoordinate;

		/// In FUnits.
		uint16 width;

		/// In FUnits.
		uint16 height;

		/// In FUnits.
		sint16 yMax;

		/// In FUnits.
		uint16 advanceWidth;

		/// In FUnits.
		sint16 leftSideBearing;

		// Spaces, tabs, etc.
		bool whitespace;
	};

	struct GlyphPair {
		uint16 firstGlyph;
		uint16 secondGlyph;

		bool operator==(GlyphPair other) const {
			return *pointer_cast<const uint32*>(this) == *pointer_cast<const uint32*>(&other);
		}
	};
}

template<>
struct std::hash<Trident::GlyphPair> {
	std::size_t operator()(Trident::GlyphPair key) const {
		return *pointer_cast<const Trident::uint32*>(&key);
	}
};