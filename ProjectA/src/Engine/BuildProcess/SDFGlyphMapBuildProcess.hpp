#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"

namespace ProjectA {
	// TODO: This struct is very similar to Glyph. Maybe merge them?
	struct SDFGlyph {
		// Top-left coordinate.
		svec2<float32> minCoordinate;

		// Bottom-right coordinate.
		svec2<float32> maxCoordinate;

		// Width of the glyph on the glyph map, in px.
		float32 width;

		// Spaces, tabs, etc.
		bool whitespace;
	};

	struct SDFGlyphMap {
		// Glyph map data in the R8_UNORM format.
		sequence<uint8> data;

		// Dimensions of the glyph map data.
		Extent2D extent;

		// Per-glyph data.
		sequence<SDFGlyph> glyphs;
	};

	// TODO: Extend Process.
	class SDFGlyphMapBuildProcess {
		public:
			static SDFGlyphMap build(const File<FileFormat::TTF>& file);

		private:
			static uint64 computeMaxGlyphByteSize(const File<FileFormat::TTF>& file);

		private:
			struct FontStorage {
				uint32 outputGlyphSize;

				// Size of the area occupied by the SDF spread.
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