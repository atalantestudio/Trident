// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	struct SceneLightingDescriptor {
		svec3<float32> skyColor;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;
		float32 sunIntensity;
	};

	struct LightmapperDescriptor {
		view<BuildVertex3D> vertices;
		view<svec2<float32>> lightmapCoordinates;
		view<uint32> indices;
		SceneLightingDescriptor lighting;
	};

	struct LightmapperContext {
		ref<ImageView> gBufferPositionImageView;
		ref<ImageView> gBufferSmoothPositionImageView;
		ref<ImageView> gBufferTriangleNormalImageView;
		ref<ImageView> gBufferVertexNormalImageView;
		ref<ImageView> intermediateLightmapAtlasImageView;
		ref<ImageView> lightmapAtlasImageView;

		ref<Image> gBufferPositionImage;
		ref<Image> gBufferSmoothPositionImage;
		ref<Image> gBufferTriangleNormalImage;
		ref<Image> gBufferVertexNormalImage;
		ref<Image> intermediateLightmapAtlasImage;
		ref<Image> lightmapAtlasImage;

		ref<Framebuffer> gBufferFramebuffer;
		ref<Framebuffer> dilationFramebuffer;

		ref<Buffer> vertexBuffer;
		ref<Buffer> indexBuffer;
		ref<Buffer> lightmapAtlasStagingBuffer;
	};

	// TODO(application/destruction-leaks)
	class Lightmapper {
		private:
			struct Arguments {
				Extent2D atlasExtent;
				uint32 triangleCount;
				uint32 _0;
				svec3<float32> skyColor;
				uint32 _1;
				svec3<float32> sunDirection;
				uint32 _2;
				svec3<float32> sunColor;
				float32 sunIntensity;
			};

		public:
			void generateAtlas(ImageAssetDescriptor& imageAssetDescriptor, const LightmapperDescriptor& descriptor);

		private:
			void initialize();

			ref<RenderPass> createGBufferRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createGBufferPositionImage() const;
			ref<Image> createGBufferSmoothPositionImage() const;
			ref<Image> createGBufferTriangleNormalImage() const;
			ref<Image> createGBufferVertexNormalImage() const;
			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;

			ref<ImageView> createGBufferImageView(ref<Image> image, view<char8> name) const;
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

			ref<Buffer> createArgumentBuffer() const;
			ref<Buffer> createVertexBuffer(uint64 vertexCount) const;
			ref<Buffer> createIndexBuffer(uint64 indexCount) const;

			ref<Sampler> createIntermediateLightmapAtlasSampler() const;

			void encodeOneTimeCommands(view<BuildVertex3D> vertices, view<svec2<float32>> lightmapCoordinates, view<uint32> indices) const;

			void generateLightmaps(uint32 vertexCount) const;

		private:
			bool initialized = false;
			ImageFormat atlasFormat;
			Extent3D atlasExtent;
			ImageAssetDescriptor imageAssetDescriptor;
			SceneLightingDescriptor lighting;

			sequence<ref<DescriptorSet>> descriptorSets;

			ref<Buffer> argumentBuffer;

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

			ref<DescriptorSetLayout> descriptorSetLayouts[4];

			ref<RenderPass> gBufferRenderPass;
			ref<RenderPass> dilationRenderPass;
	};
}