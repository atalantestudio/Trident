#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/GUI/Font/Glyph.hpp"

namespace ProjectA {
	struct SDFGlyphMap {
		sequence<uint8> data;
		Extent2D extent;
		sequence<Glyph> glyphs;
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