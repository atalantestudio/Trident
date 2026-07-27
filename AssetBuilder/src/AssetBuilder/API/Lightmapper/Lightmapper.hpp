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
		ImageFormat atlasFormat;
		Extent2D atlasExtent;
	};

	struct LightmapperContext {
		ref<ImageView> gBufferPositionImageView;
		ref<ImageView> gBufferVertexNormalImageView;
		ref<ImageView> gBufferTriangleNormalImageView;
		ref<ImageView> intermediateLightmapAtlasImageView;
		ref<ImageView> lightmapAtlasImageView;

		ref<Image> gBufferPositionImage;
		ref<Image> gBufferVertexNormalImage;
		ref<Image> gBufferTriangleNormalImage;
		ref<Image> intermediateLightmapAtlasImage;
		ref<Image> lightmapAtlasImage;

		ref<Framebuffer> gBufferFramebuffer;
		ref<Framebuffer> dilationFramebuffer;

		ref<Buffer> indexBuffer;
		ref<Buffer> vertexComponentBuffer;
		ref<Buffer> normalComponentBuffer;
		ref<Buffer> lightmapAtlasStagingBuffer;
	};

	// TODO(application/destruction-leaks)
	class Lightmapper {
		private:
			struct TraceRaysArguments {
				Extent2D atlasExtent;
				uint32 triangleCount;
			};

		public:
			void setOptions(const LightmapperOptions& options);

			void buildLightmapAtlas(const StaticScene3D& scene, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			void initialize();

			ref<RenderPass> createGBufferRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createGBufferPositionImage() const;
			ref<Image> createGBufferVertexNormalImage() const;
			ref<Image> createGBufferTriangleNormalImage() const;
			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;
			ref<ImageView> createGBufferPositionImageView();
			ref<ImageView> createGBufferVertexNormalImageView();
			ref<ImageView> createGBufferTriangleNormalImageView();
			ref<ImageView> createIntermediateLightmapAtlasImageView();
			ref<ImageView> createLightmapAtlasImageView();

			ref<Framebuffer> createGBufferFramebuffer() const;
			ref<Framebuffer> createDilationFramebuffer() const;

			ref<DescriptorSetLayout> createGBufferDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createTraceRaysDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createPostProcessingDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDilationDescriptorSetLayout() const;

			ref<GraphicsPipeline> createGBufferPipeline() const;
			ref<ComputePipeline> createTraceRaysPipeline() const;
			ref<ComputePipeline> createIntermediateDilationPipeline() const;
			ref<ComputePipeline> createHorizontalBilateralFilterPipeline() const;
			ref<ComputePipeline> createVerticalBilateralFilterPipeline() const;
			ref<GraphicsPipeline> createDilationPipeline() const;

			ref<Buffer> createTraceRaysArgumentBuffer() const;
			ref<Buffer> createIndexBuffer(uint64 size) const;
			ref<Buffer> createVertexComponentBuffer(uint64 size) const;
			ref<Buffer> createNormalComponentBuffer(uint64 size) const;

			ref<Sampler> createIntermediateLightmapAtlasSampler() const;

			void encodeOneTimeCommands(const StaticScene3D& scene) const;

			void generateLightmaps(uint32 vertexCount) const;

		private:
			bool initialized = false;
			ImageFormat atlasFormat;
			Extent2D atlasExtent;

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<Buffer> traceRaysArgumentBuffer;

			ref<Sampler> intermediateLightmapAtlasSampler;

			LightmapperContext context;

			ref<GraphicsPipeline> gBufferPipeline;
			ref<ComputePipeline> traceRaysPipeline;
			ref<ComputePipeline> intermediateDilationPipeline;
			ref<ComputePipeline> horizontalBilateralFilterPipeline;
			ref<ComputePipeline> verticalBilateralFilterPipeline;
			ref<GraphicsPipeline> dilationPipeline;

			ref<PipelineLayout> gBufferPipelineLayout;
			ref<PipelineLayout> traceRaysPipelineLayout;
			ref<PipelineLayout> postProcessingPipelineLayout;
			ref<PipelineLayout> dilationPipelineLayout;

			ref<DescriptorSetLayout> gBufferDescriptorSetLayout;
			ref<DescriptorSetLayout> traceRaysDescriptorSetLayout;
			ref<DescriptorSetLayout> postProcessingDescriptorSetLayout;
			ref<DescriptorSetLayout> dilationDescriptorSetLayout;

			ref<RenderPass> gBufferRenderPass;
			ref<RenderPass> dilationRenderPass;
	};
}