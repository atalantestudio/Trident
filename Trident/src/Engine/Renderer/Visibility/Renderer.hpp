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

		private:
			static constexpr uint32 SWAPCHAIN_ATTACHMENT_INDEX = 0;
			static constexpr uint32 VISIBILITY_ATTACHMENT_INDEX = 1;
			static constexpr uint32 MATERIAL_INDEX_ATTACHMENT_INDEX = 2;
			static constexpr uint32 DEPTH_STENCIL_ATTACHMENT_INDEX = 3;

			static constexpr uint32 VISIBILITY_SUBPASS_INDEX = 0;
			static constexpr uint32 DEBUG_SUBPASS_INDEX = 1;

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
			ref<RenderPass> _createRenderPass() const;

			ref<Subpass> createVisibilitySubpass() const;
			ref<Subpass> createDebugSubpass() const;

			ref<Image> createVisibilityImage() const;
			ref<Image> createMaterialIndexImage() const;
			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createVisibilityImageView();
			ref<ImageView> createMaterialIndexImageView();
			ref<ImageView> createDepthStencilImageView();

			void recreateFramebuffers();

			void createCameraDescriptorSetLayout();
			void createClusterDescriptorSetLayout();
			void createInstanceDescriptorSetLayout();
			void createDebugDescriptorSetLayout();

			void createVisibilityPipelineLayout();
			void createDebugPipelineLayout();
			ref<PipelineLayout> createMaterialOffsetPipelineLayout() const;

			void createVisibilityRenderPipeline();
			void createDebugRenderPipeline();
			ref<ComputePipeline> createMaterialOffsetPipeline() const;
			void createMaterialPipelines();

			void createIndirectBuffer();
			void createCameraBuffer();
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

			void allocateDescriptorSets();

			void fillSceneBuffers() const;
			void fillIndirectBuffer() const;
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
			ref<RenderPass> renderPass;

			ref<Image> visibilityImage;
			ref<Image> materialIndexImage;
			ref<Image> depthStencilImage;

			ref<ImageView> visibilityImageView;
			ref<ImageView> materialIndexImageView;
			ref<ImageView> depthStencilImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraDescriptorSetLayout;
			ref<DescriptorSetLayout> clusterDescriptorSetLayout;
			ref<DescriptorSetLayout> instanceDescriptorSetLayout;
			ref<DescriptorSetLayout> debugDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;
			ref<DescriptorSet> clusterDescriptorSet;
			ref<DescriptorSet> instanceDescriptorSet;
			ref<DescriptorSet> debugDescriptorSet;

			ref<PipelineLayout> visibilityPipelineLayout;
			ref<PipelineLayout> debugPipelineLayout;
			ref<PipelineLayout> materialOffsetPipelineLayout;

			ref<RenderPipeline> visibilityRenderPipeline;
			ref<RenderPipeline> debugRenderPipeline;
			ref<ComputePipeline> materialOffsetPipeline;

			ref<Buffer> indirectBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexNormalComponentBuffer;
			ref<Buffer> vertexTextureCoordinateComponentBuffer;
			ref<Buffer> vertexPositionIndexBuffer;
			ref<Buffer> vertexNormalIndexBuffer;
			ref<Buffer> vertexTextureCoordinateIndexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> geometryBuffer;

			// Number of pixels used per material.
			ref<Buffer> materialCountBuffer;

			// Offset of the first pixel used per material.
			ref<Buffer> materialOffsetBuffer;

			DrawIndirectCommand* indirectMappedRange;
			Camera* cameraMappedRange;
			ClusteredInstance* instanceMappedRange;

			sequence<ref<ComputePipeline>> materialPipelines;
	};
}