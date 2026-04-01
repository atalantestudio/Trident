// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace Trident {
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

			void updateInstanceTransform(Entity entity, const mat4x4<float32>& transform);

			void onEvent(Event& event) override;

		private:
			ref<RenderPass> createVisibilityRenderPass() const;
			ref<RenderPass> createPresentRenderPass() const;

			void createFrameResources();

			ref<Subpass> createVisibilitySubpass() const;
			ref<Subpass> createPresentSubpass() const;
			// ref<Subpass> createDebugPresentSubpass() const;

			ref<Framebuffer> createVisibilityFramebuffer() const;
			void createPresentFramebuffers();

			void createMaterialObjects();

			ref<DescriptorSetLayout> createCameraDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createClusterDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createInstanceDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialCountDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialOffsetDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialCoordinateDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createMaterialBaseDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createOutputDescriptorSetLayout() const;
			ref<DescriptorSetLayout> createDebugDescriptorSetLayout() const;

			ref<PipelineLayout> createVisibilityPipelineLayout() const;
			ref<PipelineLayout> createMaterialCountPipelineLayout() const;
			ref<PipelineLayout> createMaterialOffsetPipelineLayout() const;
			ref<PipelineLayout> createMaterialCoordinatePipelineLayout() const;
			ref<PipelineLayout> createOutputPipelineLayout() const;
			// ref<PipelineLayout> createDebugPipelineLayout() const;

			ref<GraphicsPipeline> createVisibilityPipeline() const;
			ref<ComputePipeline> createMaterialCountPipeline() const;
			ref<ComputePipeline> createMaterialOffsetPipeline() const;
			ref<ComputePipeline> createMaterialCoordinatePipeline() const;
			ref<GraphicsPipeline> createOutputPipeline() const;
			// ref<GraphicsPipeline> createDebugPipeline() const;

			ref<Buffer<DrawIndirectCommand>> createVisibilityIndirectBuffer() const;
			ref<Buffer<Camera>> createCameraBuffer() const;
			ref<Buffer<uint8>> createDataBuffer() const;
			ref<Buffer<ClusteredInstance>> createInstanceBuffer() const;
			void createGeometryBuffer();
			ref<Buffer<uint32>> createMaterialCountBuffer() const;
			ref<Buffer<uint32>> createMaterialOffsetBuffer() const;
			ref<Buffer<DispatchIndirectCommand>> createMaterialIndirectBuffer() const;
			ref<Buffer<uint32>> createMaterialCoordinateBuffer() const;

			ref<Image> createVisibilityImage() const;
			ref<Image> createOutputImage() const;
			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createVisibilityImageView();
			ref<ImageView> createOutputImageView();
			ref<ImageView> createDepthStencilImageView();

			ref<Sampler> createMaterialImageSampler() const;

			void allocateDescriptorSets();

			void fillSceneBuffers() const;

			void writeVisibilityIndirectCommand() const;
			void updateDataRegionSizes();
			void updateBufferDescriptorSets();
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
			ref<DescriptorSetLayout> materialCountDescriptorSetLayout;
			ref<DescriptorSetLayout> materialOffsetDescriptorSetLayout;
			ref<DescriptorSetLayout> materialCoordinateDescriptorSetLayout;
			ref<DescriptorSetLayout> materialBaseDescriptorSetLayout;
			sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts;
			ref<DescriptorSetLayout> outputDescriptorSetLayout;
			ref<DescriptorSetLayout> debugDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;
			ref<DescriptorSet> clusterDescriptorSet;
			ref<DescriptorSet> instanceDescriptorSet;
			ref<DescriptorSet> materialCountDescriptorSet;
			ref<DescriptorSet> materialOffsetDescriptorSet;
			ref<DescriptorSet> materialCoordinateDescriptorSet;
			ref<DescriptorSet> materialBaseDescriptorSet;
			sequence<ref<DescriptorSet>> materialDescriptorSets;
			ref<DescriptorSet> outputDescriptorSet;
			ref<DescriptorSet> debugDescriptorSet;

			ref<PipelineLayout> visibilityPipelineLayout;
			ref<PipelineLayout> materialCountPipelineLayout;
			ref<PipelineLayout> materialOffsetPipelineLayout;
			ref<PipelineLayout> materialCoordinatePipelineLayout;
			sequence<ref<PipelineLayout>> materialPipelineLayouts;
			ref<PipelineLayout> outputPipelineLayout;
			// ref<PipelineLayout> debugPipelineLayout;

			ref<GraphicsPipeline> visibilityPipeline;
			ref<ComputePipeline> materialCountPipeline;
			ref<ComputePipeline> materialOffsetPipeline;
			ref<ComputePipeline> materialCoordinatePipeline;
			sequence<ref<ComputePipeline>> materialPipelines;
			ref<GraphicsPipeline> outputPipeline;
			// ref<GraphicsPipeline> debugPipeline;

			ref<Buffer<DrawIndirectCommand>> visibilityIndirectBuffer;
			ref<Buffer<Camera>> cameraBuffer;

			// TODO
			// Contents:
			// - Vertex position components
			// - Vertex normal components
			// - Vertex texture coordinate components
			// - Vertex position indices
			// - Vertex normal indices
			// - Vertex texture coordinate indices
			// - Clusters
			// - Geometries
			ref<Buffer<uint8>> dataBuffer;
			ref<Buffer<ClusteredInstance>> instanceBuffer;

			// Number of pixels used per material.
			ref<Buffer<uint32>> materialCountBuffer;

			// Offset of the first pixel used per material.
			ref<Buffer<uint32>> materialOffsetBuffer;

			// Indirect arguments for material pipelines.
			ref<Buffer<DispatchIndirectCommand>> materialIndirectBuffer;

			// Screen pixels sorted by material index.
			// TODO: Use 1D image instead.
			ref<Buffer<uint32>> materialCoordinateBuffer;

			ref<Image> visibilityImage;
			sequence<sequence<ref<Image>>> materialImages;
			ref<Image> outputImage;
			ref<Image> depthStencilImage;

			ref<ImageView> visibilityImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;
			ref<ImageView> outputImageView;
			ref<ImageView> depthStencilImageView;

			ref<Sampler> materialImageSampler;

			uint64 vertexPositionComponentRegionSize;
			uint64 vertexNormalComponentRegionSize;
			uint64 vertexTextureCoordinateComponentRegionSize;
			uint64 vertexPositionIndexRegionSize;
			uint64 vertexNormalIndexRegionSize;
			uint64 vertexTextureCoordinateIndexRegionSize;
			uint64 clusterRegionSize;
			uint64 geometryRegionSize;

			Camera* cameraMappedRange;
			ClusteredInstance* instanceMappedRange;
	};
}