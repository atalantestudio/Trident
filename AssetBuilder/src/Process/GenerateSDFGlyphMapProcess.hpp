// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/File.hpp"
#include "Engine/Font/Glyph.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	struct SDFGlyphMap {
		sequence<uint8> data;
		Extent2D extent;
		sequence<Glyph> glyphs;
	};

	class GenerateSDFGlyphMapProcess {
		public:
			struct FontStorage {
				uint32 spreadFactor;
				uint32 outputGlyphSize;
				sint32 lineHeight;
			};

			struct alignas(8) GlyphBuffer {
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

		public:
			explicit GenerateSDFGlyphMapProcess(const File<FileFormat::TTF>& file, SDFGlyphMap& sdfGlyphMap);

		private:
			void createFontBuffer();
			void createGlyphBuffer(uint64 size);
			void createGlyphOffsetBuffer();

			void createGlyphImage();
			void createGlyphMapImage(Extent2D extent);

			void createGlyphImageView();
			void createGlyphMapImageView();

			void createDescriptorSetLayoutsAndSets();

			void createGlyphImagePipelineLayout();
			void createGlyphSdfPipelineLayout();

			void createGlyphImageComputePipeline();
			void createGlyphSdfComputePipeline();

			void allocateDescriptorSets();

			void updateDescriptorSets() const;

		private:
			ref<Buffer> fontBuffer;
			ref<Buffer> glyphBuffer;
			ref<Buffer> glyphOffsetBuffer;

			// Contains a high-resolution SDF for a single glyph.
			ref<Image> glyphImage;

			// Contains low-resolution SDFs for each glyph.
			ref<Image> glyphMapImage;

			ref<ImageView> glyphImageView;
			ref<ImageView> glyphMapImageView;

			ref<DescriptorSetLayout> glyphImageDescriptorSetLayout;
			ref<DescriptorSetLayout> glyphSdfDescriptorSetLayout;

			ref<DescriptorSet> glyphImageDescriptorSet;
			ref<DescriptorSet> glyphSdfDescriptorSet;

			ref<PipelineLayout> glyphImagePipelineLayout;
			ref<PipelineLayout> glyphSdfPipelineLayout;

			ref<ComputePipeline> glyphImageComputePipeline;
			ref<ComputePipeline> glyphSdfComputePipeline;

			GlyphBuffer* glyphMappedRange;
			uint32* glyphOffsetMappedRange;
	};
}