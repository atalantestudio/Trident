#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"

namespace ProjectA {
	struct SDFGlyph {
		// Top-left coordinate.
		float32 minCoordinateX;
		float32 minCoordinateY;

		// Bottom-right coordinate.
		float32 maxCoordinateX;
		float32 maxCoordinateY;

		// Width of the glyph on the map, in px.
		float32 width;

		// True for spaces, tabs, etc.
		bool invisible;
	};

	struct SDFGlyphMap {
		// Glyph map data in the R8_UNORM format.
		sequence<uint8> data;

		// Dimensions of the glyph map data.
		Extent2D extent;

		// Size of the area occupied by the SDF spread.
		float32 spreadFactor;

		// Per-glyph data.
		sequence<SDFGlyph> glyphs;
	};

	class SDFGlyphMapBuildProcess {
		public:
			static SDFGlyphMap build(const TTFFile& ttfFile);

		private:
			static uint64 computeMaxGlyphByteSize(const TTFFile& ttfFile);

		private:
			struct FontStorage {
				uint32 outputGlyphSize;
				uint32 spreadFactor;
				sint32 ascent;
				sint32 descent;
			};

			struct GlyphBuffer {
				sint32 minCoordinateX;
				sint32 minCoordinateY;
				uint32 dimension;
				uint32 edgeCount;
			};

			struct alignas(8) GlyphEdge {
				sint32 startX;
				sint32 startY;
				sint32 endX;
				sint32 endY;
				sint32 controlX;
				sint32 controlY;
				bool curved;
			};
	};
}