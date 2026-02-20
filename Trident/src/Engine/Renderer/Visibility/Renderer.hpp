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

			ref<RenderPipeline> createVisibilityPipeline() const;
			ref<ComputePipeline> createMaterialCountPipeline() const;
			ref<ComputePipeline> createMaterialOffsetPipeline() const;
			ref<ComputePipeline> createMaterialCoordinatePipeline() const;
			ref<RenderPipeline> createOutputPipeline() const;
			// ref<RenderPipeline> createDebugPipeline() const;

			ref<Buffer2<DrawIndirectCommand>> createVisibilityIndirectBuffer() const;
			ref<Buffer2<Camera>> createCameraBuffer() const;
			void createVertexPositionComponentBuffer();
			void createVertexNormalComponentBuffer();
			void createVertexTextureCoordinateComponentBuffer();
			void createVertexPositionIndexBuffer();
			void createVertexNormalIndexBuffer();
			void createVertexTextureCoordinateIndexBuffer();
			void createClusterBuffer();
			void createInstanceBuffer();
			void createGeometryBuffer();
			ref<Buffer> createMaterialCountBuffer() const;
			ref<Buffer> createMaterialOffsetBuffer() const;
			ref<Buffer> createMaterialIndirectBuffer() const;
			ref<Buffer> createMaterialCoordinateBuffer() const;

			ref<Image> createVisibilityImage() const;
			ref<Image> createOutputImage() const;
			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createVisibilityImageView();
			ref<ImageView> createOutputImageView();
			ref<ImageView> createDepthStencilImageView();

			ref<Sampler> createMaterialImageSampler() const;

			void allocateDescriptorSets();

			void fillSceneBuffers() const;
			void fillVertexPositionComponentBuffer(float32* mappedRange) const;
			void fillVertexNormalComponentBuffer(float32* mappedRange) const;
			void fillVertexTextureCoordinateComponentBuffer(float32* mappedRange) const;
			void fillVertexPositionIndexBuffer(uint32* mappedRange) const;
			void fillVertexNormalIndexBuffer(uint32* mappedRange) const;
			void fillVertexTextureCoordinateIndexBuffer(uint32* mappedRange) const;
			void fillClusterBuffer(Cluster* mappedRange) const;
			void fillInstanceBuffer(ClusteredInstance* mappedRange) const;
			void fillGeometryBuffer(Geometry* mappedRange) const;

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

			ref<RenderPipeline> visibilityPipeline;
			ref<ComputePipeline> materialCountPipeline;
			ref<ComputePipeline> materialOffsetPipeline;
			ref<ComputePipeline> materialCoordinatePipeline;
			sequence<ref<ComputePipeline>> materialPipelines;
			ref<RenderPipeline> outputPipeline;
			// ref<RenderPipeline> debugPipeline;

			ref<Buffer2<DrawIndirectCommand>> visibilityIndirectBuffer;
			ref<Buffer2<Camera>> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexNormalComponentBuffer;
			ref<Buffer> vertexTextureCoordinateComponentBuffer;
			ref<Buffer> vertexPositionIndexBuffer;
			ref<Buffer> vertexNormalIndexBuffer;
			ref<Buffer> vertexTextureCoordinateIndexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer2<ClusteredInstance>> instanceBuffer;
			ref<Buffer> geometryBuffer;

			// Number of pixels used per material.
			ref<Buffer> materialCountBuffer;

			// Offset of the first pixel used per material.
			ref<Buffer> materialOffsetBuffer;

			// Indirect arguments for material pipelines.
			ref<Buffer> materialIndirectBuffer;

			// Screen pixels sorted by material index.
			// TODO: Use 1D image instead.
			ref<Buffer> materialCoordinateBuffer;

			ref<Image> visibilityImage;
			sequence<sequence<ref<Image>>> materialImages;
			ref<Image> outputImage;
			ref<Image> depthStencilImage;

			ref<ImageView> visibilityImageView;
			sequence<ref<ImageView>> materialReferenceBindingImageViews;
			ref<ImageView> outputImageView;
			ref<ImageView> depthStencilImageView;

			ref<Sampler> materialImageSampler;

			Camera* cameraMappedRange;
			ClusteredInstance* instanceMappedRange;
	};
}