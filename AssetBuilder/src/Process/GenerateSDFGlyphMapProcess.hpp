#pragma once

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Font/Glyph.hpp"
#include "Engine/Process/Process.hpp"

namespace ProjectA {
	struct SDFGlyphMap {
		sequence<uint8> data;
		Extent2D extent;
		sequence<Glyph> glyphs;
	};

	class GenerateSDFGlyphMapProcess : public Process {
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

		private:
			static constexpr uint64 MAX_GLYPH_COUNT = 2048;
			static constexpr Extent2D GLYPH_MAP_IMAGE_EXTENT = {2048, 2048};
			static constexpr uint32 GLYPH_SUBIMAGE_DIMENSION = 64;
			static constexpr uint32 SPREAD_FACTOR = 0;

		private:
			static uint64 computeMaxGlyphByteSize(const sequence<TrueTypeFont::Glyph>& glyphs);

		public:
			explicit GenerateSDFGlyphMapProcess(const File<FileFormat::TTF>& file, SDFGlyphMap& sdfGlyphMap);

		private:
			void createFontBuffer();
			void createGlyphBuffer(uint64 size);
			void createGlyphOffsetBuffer();

			void createGlyphImage();
			void createGlyphMapImage();

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

			// Used for generating the distance field for each glyph.
			ref<Image> glyphImage;

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

			FontStorage* fontMappedRange;
			GlyphBuffer* glyphMappedRange;
			uint32* glyphOffsetMappedRange;
	};
}