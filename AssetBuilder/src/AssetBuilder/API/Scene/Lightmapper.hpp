// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "AssetBuilder/API/Scene/Lightmap.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"

namespace Trident {
	struct LightmapNode;

	class Lightmapper {
		private:
			struct TraceRaysArguments {
				Extent2D atlasSize;
				uint32 lightmapPixelCount;
				uint32 triangleCount;
				DispatchIndirectCommand traceRaysCommand;
			};

			struct LightmapPixel {
				uint32 triangleIndex;
				uint32 coordinate;
			};

		public:
			static constexpr Extent2D ATLAS_SIZE = {1024, 1024};

		public:
			static LightmapNode* insertNode(Lightmap& lightmap, LightmapNode* node);

			static void updateAtlasCoordinates(StaticScene3D& scene, sequence<Lightmap>& lightmaps, view<LightmapIndex> lightmapIndices);

			void buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			ref<RenderPass> createRasterizeLightmapsRenderPass(ImageFormat lightmapAtlasImageFormat) const;

			ref<Image> createLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const;
			ref<ImageView> createLightmapAtlasImageView();

			ref<Framebuffer> createRasterizeLightmapsFramebuffer() const;

			ref<DescriptorSetLayout> createRasterizeLightmapsDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createTraceRaysDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPostProcessingDescriptorSetLayout() const;

			ref<PipelineLayout> createRasterizeLightmapsPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const;
			ref<PipelineLayout> createTraceRaysPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const;
			ref<PipelineLayout> createPostProcessingPipelineLayout(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) const;

			ref<GraphicsPipeline> createRasterizeLightmapsPipeline() const;
			ref<ComputePipeline> createTraceRaysPipeline() const;
			ref<ComputePipeline> createPostProcessingPipeline() const;

			ref<Buffer> createLightmapCoordinateBuffer(uint64 size) const;
			ref<Buffer> createLightmapCoordinateIndexBuffer(uint64 size) const;
			ref<Buffer> createTraceRaysArgumentBuffer() const;
			ref<Buffer> createLightmapPixelBuffer() const;
			ref<Buffer> createTriangleBuffer(uint64 size) const;
			ref<Buffer> createVertexComponentBuffer(uint64 size) const;
			ref<Buffer> createNormalComponentBuffer(uint64 size) const;

			void encodeOneTimeCommands(const StaticScene3D& scene) const;

		private:
			ref<RenderPass> rasterizeLightmapsRenderPass;

			ref<Image> lightmapAtlasImage;
			ref<ImageView> lightmapAtlasImageView;

			ref<Framebuffer> rasterizeLightmapsFramebuffer;

			ref<PipelineLayout> rasterizeLightmapsPipelineLayout;
			ref<PipelineLayout> traceRaysPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;

			ref<DescriptorSet> rasterizeLightmapsDescriptorSet;
			ref<DescriptorSet> traceRaysDescriptorSet;
			ref<DescriptorSet> postProcessingDescriptorSet;

			ref<GraphicsPipeline> rasterizeLightmapsPipeline;
			ref<ComputePipeline> traceRaysPipeline;
			ref<ComputePipeline> postProcessingPipeline;

			ref<Buffer> lightmapCoordinateBuffer;
			ref<Buffer> lightmapCoordinateIndexBuffer;
			ref<Buffer> traceRaysArgumentBuffer;
			ref<Buffer> lightmapPixelBuffer;
			ref<Buffer> triangleBuffer;
			ref<Buffer> vertexComponentBuffer;
			ref<Buffer> normalComponentBuffer;
	};
}