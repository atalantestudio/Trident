// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"

namespace Trident {
	struct LightmapperOptions {
		ImageFormat format;
		Extent2D atlasExtent;
	};

	struct LightmapperContext {
		ref<ImageView> intermediateLightmapAtlasImageView;
		ref<ImageView> lightmapAtlasImageView;

		ref<Image> intermediateLightmapAtlasImage;
		ref<Image> lightmapAtlasImage;

		ref<Framebuffer> rasterizationFramebuffer;
		ref<Framebuffer> dilationFramebuffer;

		ref<Buffer> lightmapCoordinateBuffer;
		ref<Buffer> triangleBuffer;
		ref<Buffer> vertexComponentBuffer;
		ref<Buffer> normalComponentBuffer;
		ref<Buffer> lightmapAtlasStagingBuffer;
	};

	// TODO(application/destruction-leaks)
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
			void setOptions(const LightmapperOptions& options);

			void buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			void initialize();

			ref<RenderPass> createRasterizationRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;
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
			bool initialized = false;
			ImageFormat format;
			Extent2D atlasExtent;

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<Buffer> traceRaysArgumentBuffer;
			ref<Buffer> luxelBuffer;

			ref<Sampler> intermediateLightmapAtlasSampler;

			LightmapperContext context;

			ref<GraphicsPipeline> rasterizationPipeline;
			ref<ComputePipeline> traceRaysPipeline;
			ref<ComputePipeline> intermediateDilationPipeline;
			ref<ComputePipeline> horizontalBilateralFilterPipeline;
			ref<ComputePipeline> verticalBilateralFilterPipeline;
			ref<GraphicsPipeline> dilationPipeline;

			ref<PipelineLayout> rasterizationPipelineLayout;
			ref<PipelineLayout> traceRaysPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;
			ref<PipelineLayout> dilationPipelineLayout;

			ref<DescriptorSetLayout> rasterizationDescriptorSetLayout;
			ref<DescriptorSetLayout> traceRaysDescriptorSetLayout;
			ref<DescriptorSetLayout> postProcessingDescriptorSetLayout;
			ref<DescriptorSetLayout> dilationDescriptorSetLayout;

			ref<RenderPass> rasterizationRenderPass;
			ref<RenderPass> dilationRenderPass;
	};
}