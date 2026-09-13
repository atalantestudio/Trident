// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/Font/Glyph.hpp"

#include "AssetBuilder/API/Font/SDFGlyphMap.hpp"
#include "../../../File/TTF.hpp"

namespace Trident {
	class GlyphRasterizer {
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
			void initialize(const GraphicsAdapter* adapter);

			void generateGlyphAtlas(const File<FileFormat::TTF>& file, SDFGlyphMap& sdfGlyphMap);

		private:
			// TODO(asset-builder/font/sdf): Move inside the engine.
			ref<ShaderModule> loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const;

			ref<Buffer> createFontBuffer() const;
			ref<Buffer> createGlyphBuffer(uint64 size) const;
			ref<Buffer> createGlyphOffsetBuffer() const;

			ref<Image> createGlyphImage() const;
			ref<Image> createGlyphMapImage(Extent2D extent) const;

			ref<ImageView> createGlyphImageView() const;
			ref<ImageView> createGlyphMapImageView() const;

			ref<DescriptorSetLayout> createGlyphImageDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createGlyphSdfDescriptorSetLayout() const;

			ref<PipelineLayout> createGlyphImagePipelineLayout() const;
			ref<PipelineLayout> createGlyphSdfPipelineLayout() const;

			ref<ComputePipeline> createGlyphImageComputePipeline() const;
			ref<ComputePipeline> createGlyphSdfComputePipeline() const;

			void updateDescriptorSets() const;

		private:
			const GraphicsAdapter* adapter = nullptr;

			ref<Instance> instance;
			ref<PhysicalDevice> physicalDevice;
			ref<Device> device;

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

			GlyphBuffer* glyphMappedRange = nullptr;
			uint32* glyphOffsetMappedRange = nullptr;
	};
}