#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Engine/Renderer/ImageLayout.hpp"

namespace ProjectA {
	struct SDFGlyph {
		// Horizontal offset of the glyph on the map, in px.
		uint32 xOffset;

		// Width of the glyph on the map, in px.
		uint32 width;

		float32 leftSideBearing;
		float32 advanceWidth;

		// Index of the line the glyph is on.
		// Can be multiplied by the map's line height to get the Y offset.
		uint32 lineIndex;

		// True for spaces, tabs, etc.
		bool invisible;
	};

	struct SDFGlyphMap {
		ref<Image> glyphMap;

		// Height of a line on the map, in px.
		uint32 lineHeight;

		// Width of the pixels used by the SDF gradient.
		uint32 spreadFactor;

		chain<SDFGlyph> glyphs;
	};

	class SDFGlyphMapBuildProcess {
		public:
			static SDFGlyphMap build(const TTFFile& ttfFile);

		private:
			static void transitionImage(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout);

			static uint64 computeMaxGlyphByteSize(const TTFFile& ttfFile);

		private:
			struct alignas(8) GlyphEdge {
				sint32 startX;
				sint32 startY;
				sint32 endX;
				sint32 endY;
				sint32 controlX;
				sint32 controlY;
				bool curved;
			};

			struct GlyphBuffer {
				sint32 minCoordinateX;
				sint32 minCoordinateY;
				sint32 maxCoordinateX;
				sint32 maxCoordinateY;
				uint32 advanceWidth;
				sint32 leftBearing;
				chain<GlyphEdge> edges;
			};
	};
}