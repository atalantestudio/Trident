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
				Extent2D atlasExtent;
				uint32 luxelCount;
				uint32 triangleCount;
				DispatchIndirectCommand traceRaysCommand;
			};

			struct Luxel {
				uint32 triangleIndex;
				uint32 coordinate;
			};

		public:
			explicit Lightmapper(Extent2D atlasExtent);

			void buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			ref<RenderPass> createRasterizationRenderPass(ImageFormat lightmapAtlasImageFormat) const;
			ref<RenderPass> createDilationRenderPass(ImageFormat lightmapAtlasImageFormat) const;

			ref<Image> createIntermediateLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const;
			ref<Image> createLightmapAtlasImage(const ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor) const;
			ref<ImageView> createIntermediateLightmapAtlasImageView();
			ref<ImageView> createLightmapAtlasImageView();

			ref<Framebuffer> createRasterizationFramebuffer() const;
			ref<Framebuffer> createDilationFramebuffer() const;

			ref<DescriptorSetLayout> createRasterizationDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createTraceRaysDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPostProcessingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDilationDescriptorSetLayout() const;

			ref<GraphicsPipeline> createRasterizationPipeline() const;
			ref<ComputePipeline> createTraceRaysPipeline() const;
			ref<ComputePipeline> createIntermediateDilationPipeline() const;
			ref<ComputePipeline> createHorizontalBilateralFilterPipeline() const;
			ref<ComputePipeline> createVerticalBilateralFilterPipeline() const;
			ref<GraphicsPipeline> createDilationPipeline() const;

			ref<Buffer> createLightmapCoordinateBuffer(uint64 size) const;
			ref<Buffer> createTraceRaysArgumentBuffer() const;
			ref<Buffer> createLuxelBuffer() const;
			ref<Buffer> createTriangleBuffer(uint64 size) const;
			ref<Buffer> createVertexComponentBuffer(uint64 size) const;
			ref<Buffer> createNormalComponentBuffer(uint64 size) const;

			ref<Sampler> createIntermediateLightmapAtlasSampler() const;

			void encodeOneTimeCommands(const StaticScene3D& scene) const;

			void generateLightmaps(uint32 vertexCount) const;

		private:
			Extent2D atlasExtent;

			ref<RenderPass> rasterizationRenderPass;
			ref<RenderPass> dilationRenderPass;

			ref<Image> intermediateLightmapAtlasImage;
			ref<Image> lightmapAtlasImage;
			ref<ImageView> intermediateLightmapAtlasImageView;
			ref<ImageView> lightmapAtlasImageView;

			ref<Framebuffer> rasterizationFramebuffer;
			ref<Framebuffer> dilationFramebuffer;

			ref<PipelineLayout> rasterizationPipelineLayout;
			ref<PipelineLayout> traceRaysPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;
			ref<PipelineLayout> dilationPipelineLayout;

			ref<DescriptorSet> rasterizationDescriptorSet;
			ref<DescriptorSet> traceRaysDescriptorSet;
			ref<DescriptorSet> postProcessingDescriptorSet;
			ref<DescriptorSet> dilationDescriptorSet;

			ref<GraphicsPipeline> rasterizationPipeline;
			ref<ComputePipeline> traceRaysPipeline;
			ref<ComputePipeline> intermediateDilationPipeline;
			ref<ComputePipeline> horizontalBilateralFilterPipeline;
			ref<ComputePipeline> verticalBilateralFilterPipeline;
			ref<GraphicsPipeline> dilationPipeline;

			ref<Buffer> lightmapCoordinateBuffer;
			ref<Buffer> traceRaysArgumentBuffer;
			ref<Buffer> luxelBuffer;
			ref<Buffer> triangleBuffer;
			ref<Buffer> vertexComponentBuffer;
			ref<Buffer> normalComponentBuffer;
			ref<Buffer> lightmapAtlasStagingBuffer;

			ref<Sampler> intermediateLightmapAtlasSampler;
	};
}