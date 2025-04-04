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
			void createGlyphBuffers();
			void createGlyphImage();
			void createGlyphImageComputePipeline();
			void createGlyphMapSdfImage();
			void createGlyphMapSdfSampler();
			void createGlyphSdfImageComputePipeline();
			void createVertexBuffer();
			void createIndexBuffer();
			void createGlyphMapSdfImageRenderPipeline();

			void transitionGlyphImageCompute() const;
			void transitionGlyphImageRender() const;
			void transitionGlyphMapSdfImageCompute() const;
			void transitionGlyphMapSdfImageRender() const;

			void buildGlyphMapSdfImage() const;

			uint64 computeMaxGlyphSize() const;

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

		private:
			const TTFFile& ttfFile;

			ref<Buffer> glyphStagingBuffer;
			ref<Buffer> glyphStorageBuffer;
			ref<Buffer> glyphIndexStagingBuffer;
			ref<Buffer> glyphIndexStorageBuffer;

			uint64 maxGlyphSize;

			sint32* glyphMappedRange;
			uint32* glyphIndexMappedRange;

			// High-resolution glyph.
			ref<Image> glyphImage;
			ref<ImageView> glyphImageView;

			ref<PipelineLayout> glyphImageComputePipelineLayout;
			ref<ComputePipeline> glyphImageComputePipeline;

			// Glyph map signed distance field.
			// One glyph is 64x64 on this image.
			ref<Image> glyphMapSdfImage;
			ref<ImageView> glyphMapSdfImageView;
			ref<Sampler> glyphMapSdfSampler;

			ref<PipelineLayout> glyphSdfImageComputePipelineLayout;
			ref<ComputePipeline> glyphSdfImageComputePipeline;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
	};
}