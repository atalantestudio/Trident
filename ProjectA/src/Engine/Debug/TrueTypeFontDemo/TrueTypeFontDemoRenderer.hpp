#pragma once

#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TrueTypeFontDemoRenderer : public Renderer {
		public:
			TrueTypeFontDemoRenderer(const TTFFile& ttfFile);

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

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
				chain<GlyphEdge> edges;
			};

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> glyphEdgeStorageBuffer;

			// This image is written to by the compute shader.

			// High-resolution glyph.
			ref<Image> glyphImage;
			ref<ImageView> glyphImageView;

			ref<Sampler> sampler;

			// Glyph signed distance field image.
			ref<Image> glyphSdfImage;
	};
}