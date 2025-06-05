#pragma once

#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
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
		ref<Image> glyphMap;

		// Width of the pixels used by the SDF gradient.
		float32 spreadFactor;

		std::vector<SDFGlyph> glyphs;
	};

	struct FontStorage {
		uint32 outputGlyphSize;
		uint32 spreadFactor;
		sint32 ascent;
		sint32 descent;
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
				uint32 dimension;
				uint32 edgeCount;
			};
	};
}