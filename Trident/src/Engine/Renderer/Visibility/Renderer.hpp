// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
	#define ENABLE_VISIBILITY_DEBUG_PASS

	class VisibilityRenderer : public SceneRenderer<ClusteredScene> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
				float32 nearPlane;
				float32 farPlane;
				Extent2D viewport;
				vec3<float32> position;
			};

			struct Geometry {
				uint32 vertexOffset;
				uint32 clusterCount;
			};

		public:
			void build() override;

			void postSetScene() override;

			void postSetCameraController() override;

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCameraPosition() const;

			void updateCameraProjectionView() const;

			void updateViewport() const;

			void updateInstanceWorld(Entity entity, const mat4x4<float32>& world);

			void onEvent(Event& event) override;

		private:
			ref<RenderPass> createVisibilityRenderPass() const;
			ref<RenderPass> createPresentRenderPass() const;

			void createFrameResources();

			ref<Subpass> createVisibilitySubpass() const;
			ref<Subpass> createPresentSubpass() const;

			ref<Framebuffer> createVisibilityFramebuffer() const;
			void createPresentFramebuffers();

			void createMaterialObjects();
			void initializeMaterialShaders(uint32& imageCount, uint32& imageAssetCount, uint64& imageStagingBufferSize);
			void initializeMaterialImages(sequence<ImageMemoryBarrier>& imageMemoryBarriers, sequence<DescriptorBuffer>& materialOffsetDescriptorBuffers, sequence<DescriptorImage>& descriptorImages, sequence<WriteDescriptorSet>& writeDescriptorSets);

			ref<DescriptorSetLayout> createCameraDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createClusterDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createInstanceDescriptorSetLayout() const;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<DescriptorSetLayout> createVisibilityDebugDescriptorSetLayout() const;
			#endif

			ref<DescriptorSetLayout> createMaterialCountDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialOffsetDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialCoordinateDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialBaseDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createOutputDescriptorSetLayout() const;

			ref<PipelineLayout> createVisibilityPipelineLayout() const;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<PipelineLayout> createVisibilityDebugPipelineLayout() const;
			#endif

			ref<PipelineLayout> createMaterialCountPipelineLayout() const;
			ref<PipelineLayout> createMaterialOffsetPipelineLayout() const;
			ref<PipelineLayout> createMaterialCoordinatePipelineLayout() const;
			ref<PipelineLayout> createOutputPipelineLayout() const;

			ref<GraphicsPipeline> createVisibilityPipeline() const;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<ComputePipeline> createVisibilityDebugPipeline() const;
			#endif

			ref<ComputePipeline> createMaterialCountPipeline() const;
			ref<ComputePipeline> createMaterialOffsetPipeline() const;
			ref<ComputePipeline> createMaterialCoordinatePipeline() const;
			ref<GraphicsPipeline> createOutputPipeline() const;

			ref<Buffer<DrawIndirectCommand>> createVisibilityIndirectBuffer() const;
			ref<Buffer<Camera>> createCameraBuffer() const;
			ref<Buffer<uint8>> createDataBuffer(uint64 size) const;
			ref<Buffer<ClusteredInstance>> createInstanceBuffer() const;
			ref<Buffer<uint32>> createMaterialCountBuffer() const;
			ref<Buffer<uint32>> createMaterialOffsetBuffer() const;
			ref<Buffer<DispatchIndirectCommand>> createMaterialShaderIndirectBuffer() const;
			ref<Buffer<uint32>> createMaterialCoordinateBuffer() const;

			ref<Image> createVisibilityImage() const;
			ref<Image> createDepthStencilImage() const;
			ref<Image> createOutputImage() const;

			ref<ImageView> createVisibilityImageView();
			ref<ImageView> createDepthStencilImageView();
			ref<ImageView> createOutputImageView();

			ref<Sampler> createMaterialShaderImageSampler() const;

			void allocateDescriptorSets();

			void transitionOutputImage() const;

			void writeVisibilityIndirectCommand() const;
			void updateBufferDescriptorSets(uint64 vertexPositionComponentRegionSize, uint64 vertexNormalComponentRegionSize, uint64 vertexTextureCoordinateComponentRegionSize, uint64 vertexPositionIndexRegionSize, uint64 vertexNormalIndexRegionSize, uint64 vertexTextureCoordinateIndexRegionSize, uint64 clusterRegionSize, uint64 geometryRegionSize);
			void updateDebugInputAttachmentDescriptorSets();

			bool onWindowResize(WindowResizeEvent& event);

		private:
			ref<RenderPass> visibilityRenderPass;
			ref<RenderPass> presentRenderPass;

			ref<Framebuffer> visibilityFramebuffer;
			sequence<ref<Framebuffer>> presentFramebuffers;

			ref<DescriptorSetLayout> cameraDescriptorSetLayout;
			ref<DescriptorSetLayout> clusterDescriptorSetLayout;
			ref<DescriptorSetLayout> instanceDescriptorSetLayout;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<DescriptorSetLayout> visibilityDebugDescriptorSetLayout;
			#endif

			ref<DescriptorSetLayout> materialCountDescriptorSetLayout;
			ref<DescriptorSetLayout> materialOffsetDescriptorSetLayout;
			ref<DescriptorSetLayout> materialCoordinateDescriptorSetLayout;
			ref<DescriptorSetLayout> materialBaseDescriptorSetLayout;
			sequence<ref<DescriptorSetLayout>> materialShaderUserDescriptorSetLayouts;
			ref<DescriptorSetLayout> outputDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;
			ref<DescriptorSet> clusterDescriptorSet;
			ref<DescriptorSet> instanceDescriptorSet;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<DescriptorSet> visibilityDebugDescriptorSet;
			#endif

			ref<DescriptorSet> materialCountDescriptorSet;
			ref<DescriptorSet> materialOffsetDescriptorSet;
			ref<DescriptorSet> materialCoordinateDescriptorSet;
			ref<DescriptorSet> materialBaseDescriptorSet;
			sequence<ref<DescriptorSet>> materialShaderUserDescriptorSets;
			ref<DescriptorSet> outputDescriptorSet;

			ref<PipelineLayout> visibilityPipelineLayout;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<PipelineLayout> visibilityDebugPipelineLayout;
			#endif

			ref<PipelineLayout> materialCountPipelineLayout;
			ref<PipelineLayout> materialOffsetPipelineLayout;
			ref<PipelineLayout> materialCoordinatePipelineLayout;
			sequence<ref<PipelineLayout>> materialShaderPipelineLayouts;
			ref<PipelineLayout> outputPipelineLayout;

			ref<GraphicsPipeline> visibilityPipeline;

			#if defined(ENABLE_VISIBILITY_DEBUG_PASS)
				ref<ComputePipeline> visibilityDebugPipeline;
			#endif

			ref<ComputePipeline> materialCountPipeline;
			ref<ComputePipeline> materialOffsetPipeline;
			ref<ComputePipeline> materialCoordinatePipeline;
			sequence<ref<ComputePipeline>> materialShaderPipelines;
			ref<GraphicsPipeline> outputPipeline;

			ref<Buffer<DrawIndirectCommand>> visibilityIndirectBuffer;
			ref<Buffer<Camera>> cameraBuffer;

			// Vertex position components, vertex normal components, vertex texture coordinate components,
			// vertex position indices, vertex normal indices, vertex texture coordinate indices, clusters and geometries.
			ref<Buffer<uint8>> dataBuffer;

			ref<Buffer<ClusteredInstance>> instanceBuffer;

			// Number of pixels used per material.
			ref<Buffer<uint32>> materialCountBuffer;

			// Offset of the first pixel used per material.
			ref<Buffer<uint32>> materialOffsetBuffer;

			// Indirect arguments for material shader pipelines.
			ref<Buffer<DispatchIndirectCommand>> materialShaderIndirectBuffer;

			// TODO: Use a 1D image instead.
			// Screen pixels sorted by material index.
			ref<Buffer<uint32>> materialCoordinateBuffer;

			ref<Image> visibilityImage;
			ref<Image> depthStencilImage;
			sequence<ref<Image>> materialShaderImages;
			ref<Image> outputImage;

			ref<ImageView> visibilityImageView;
			ref<ImageView> depthStencilImageView;
			sequence<ref<ImageView>> materialShaderImageViews;
			ref<ImageView> outputImageView;

			// NOTE: Also used for the depth stencil image in the visibility debug pass.
			ref<Sampler> materialShaderImageSampler;

			Camera* cameraMappedRange;
			ClusteredInstance* instanceMappedRange;
	};
}