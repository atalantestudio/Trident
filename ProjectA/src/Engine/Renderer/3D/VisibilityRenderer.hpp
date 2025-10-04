#pragma once

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace ProjectA {
	class VisibilityRenderer : public SceneRenderer<ClusteredScene> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
			};

			struct Vertex {
				uint32 positionIndex;
			};

			struct Cluster {
				uint32 instanceIndex;
			};

			struct Instance {
				mat4x4<float32> transform;
				uint32 geometryIndex;
			};

			struct Geometry {
				uint32 vertexBufferOffset;
			};

		private:
			static constexpr uint32 VISIBILITY_SUBPASS_INDEX = 0;
			static constexpr uint32 SCREEN_SUBPASS_INDEX = 1;

		public:
			void build() override;

			void setScene(const ClusteredScene* scene) override;

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCamera();

			void updateInstanceTransform(Entity entity, const mat4x4<float32>& transform);

		private:
			void setupRenderPass();

			void recreateVisibilityImage();
			void recreateVisibilityImageView();
			void recreateDepthStencilImage();
			void recreateDepthStencilImageView();
			void recreateFramebuffers();

			void createCameraDescriptorSetLayout();
			void createClusterDescriptorSetLayout();
			void createInstanceDescriptorSetLayout();
			void createVisibilityDescriptorSetLayout();
			void createVisibilityPipelineLayout();
			void createScreenPipelineLayout();
			void createVisibilityRenderPipeline();
			void createScreenRenderPipeline();
			void createIndirectBuffer();
			void createCameraBuffer();
			void createVertexPositionComponentBuffer();
			void createVertexBuffer();
			void createClusterBuffer();
			void createInstanceBuffer();
			void createGeometryBuffer();

			void allocateDescriptorSets();

			void fillSceneBuffers();

			void updateAllDescriptorSets();
			void updateVisibilityDescriptorSet();

		private:
			ref<RenderPass> renderPass;

			ref<Image> visibilityImage;
			ref<ImageView> visibilityImageView;

			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraDescriptorSetLayout;
			ref<DescriptorSetLayout> clusterDescriptorSetLayout;
			ref<DescriptorSetLayout> instanceDescriptorSetLayout;
			ref<DescriptorSetLayout> visibilityDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;
			ref<DescriptorSet> clusterDescriptorSet;
			ref<DescriptorSet> instanceDescriptorSet;
			ref<DescriptorSet> visibilityDescriptorSet;

			ref<PipelineLayout> visibilityPipelineLayout;
			ref<PipelineLayout> screenPipelineLayout;

			ref<RenderPipeline> visibilityRenderPipeline;
			ref<RenderPipeline> screenRenderPipeline;

			ref<Buffer> indirectBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> geometryBuffer;

			uint32* indirectMappedRange;
			Camera* cameraMappedRange;
			Instance* instanceMappedRange;
	};
}