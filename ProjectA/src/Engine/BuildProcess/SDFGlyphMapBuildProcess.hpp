#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Renderer/Image.hpp"
#include "Engine/Renderer/ImageLayout.hpp"

namespace ProjectA {
	class SDFGlyphMapBuildProcess {
		public:
			static ref<Image> build(const TTFFile& file);

		private:
			static void transitionImage(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout);

			static uint64 computeMaxGlyphSize(const TTFFile& file);

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