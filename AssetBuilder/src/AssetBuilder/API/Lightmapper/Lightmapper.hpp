// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	struct LightmapperArguments {
		svec3<float32> skyColor;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;
		float32 sunIntensity;
	};

	struct LightmapperContext {
		ref<ImageView> gBufferPositionImageView;
		ref<ImageView> gBufferTriangleNormalImageView;
		ref<ImageView> gBufferVertexNormalImageView;
		ref<ImageView> intermediateLightmapAtlasImageView;
		ref<ImageView> lightmapAtlasImageView;

		ref<Image> gBufferPositionImage;
		ref<Image> gBufferTriangleNormalImage;
		ref<Image> gBufferVertexNormalImage;
		ref<Image> intermediateLightmapAtlasImage;
		ref<Image> lightmapAtlasImage;

		ref<Framebuffer> gBufferFramebuffer;
		ref<Framebuffer> dilationFramebuffer;

		ref<Buffer> vertexBuffer;
		ref<Buffer> positionBuffer;
		ref<Buffer> normalBuffer;
		ref<Buffer> lightmapAtlasStagingBuffer;
	};

	// TODO(application/destruction-leaks)
	class Lightmapper {
		private:
			struct Vertex {
				svec2<float32> lightmapCoordinate;
				uint32 positionIndex;
				uint32 normalIndex;
			};

			struct TraceRaysArguments {
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
			void buildLightmapAtlas(view<GeometryIndex3D> vertices, view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> lightmapCoordinates, const LightmapperArguments& lightmapperArguments, ImageAssetDescriptor& lightmapAtlasImageAssetDescriptor);

		private:
			void initialize();

			ref<RenderPass> createGBufferRenderPass() const;
			ref<RenderPass> createDilationRenderPass() const;

			ref<Image> createGBufferPositionImage() const;
			ref<Image> createGBufferTriangleNormalImage() const;
			ref<Image> createGBufferVertexNormalImage() const;
			ref<Image> createIntermediateLightmapAtlasImage() const;
			ref<Image> createLightmapAtlasImage() const;
			ref<ImageView> createGBufferPositionImageView();
			ref<ImageView> createGBufferTriangleNormalImageView();
			ref<ImageView> createGBufferVertexNormalImageView();
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
			ref<Buffer> createVertexBuffer(uint64 vertexCount) const;
			ref<Buffer> createPositionBuffer(uint64 positionCount) const;
			ref<Buffer> createNormalBuffer(uint64 normalCount) const;

			ref<Sampler> createIntermediateLightmapAtlasSampler() const;

			void encodeOneTimeCommands(view<GeometryIndex3D> vertices, view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> lightmapCoordinates) const;

			void generateLightmaps(uint32 vertexCount) const;

		private:
			bool initialized = false;
			ImageFormat atlasFormat;
			Extent3D atlasExtent;
			ImageAssetDescriptor imageAssetDescriptor;
			LightmapperArguments arguments;

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