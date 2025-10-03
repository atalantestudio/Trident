#pragma once

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Renderer/3D/Renderer3D.hpp"

namespace ProjectA {
	class VisibilityRenderer : public Renderer3D<ClusteredScene> {
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
			static constexpr uint32 PRESENT_SUBPASS_INDEX = 1;

		public:
			void build() override {
				updateRenderPass();
				updateVisibilityImage();
				updateVisibilityImageView();
				updateDepthStencilImage();
				updateDepthStencilImageView();
				recreateFramebuffers();

				createCameraDescriptorSetLayout();
				createClusterDescriptorSetLayout();
				createInstanceDescriptorSetLayout();
				createVisibilityDescriptorSetLayout();

				allocateDescriptorSets();

				createVisibilityPipelineLayout();
				createPresentPipelineLayout();

				createVisibilityRenderPipeline();
				createPresentRenderPipeline();

				createIndirectBuffer();
				createCameraBuffer();
			}

			void setScene(const ClusteredScene* scene) override {
				Renderer3D::setScene(scene);

				createVertexPositionComponentBuffer();
				createVertexBuffer();
				createClusterBuffer();
				createInstanceBuffer();
				createGeometryBuffer();

				updateSceneBuffers();

				updateAllDescriptorSets();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void resize(WindowResizeEvent& event) override {
				updateVisibilityImage();
				updateVisibilityImageView();

				updateDepthStencilImage();
				updateDepthStencilImageView();

				recreateFramebuffers();

				updateCamera();

				updateVisibilityDescriptorSet();
			}

			void updateCamera();

			void updateInstanceTransform(Entity entity, const mat4x4<float32>& transform);

		private:
			void updateRenderPass();
			void updateVisibilityImage();
			void updateVisibilityImageView();
			void updateDepthStencilImage();
			void updateDepthStencilImageView();
			void updateSceneBuffers();
			void recreateFramebuffers();
			void updateAllDescriptorSets();
			void updateVisibilityDescriptorSet();

			void allocateDescriptorSets();

			void createCameraDescriptorSetLayout();
			void createClusterDescriptorSetLayout();
			void createInstanceDescriptorSetLayout();
			void createVisibilityDescriptorSetLayout();
			void createVisibilityPipelineLayout();
			void createPresentPipelineLayout();
			void createVisibilityRenderPipeline();
			void createPresentRenderPipeline();
			void createIndirectBuffer();
			void createCameraBuffer();
			void createVertexPositionComponentBuffer();
			void createVertexBuffer();
			void createClusterBuffer();
			void createInstanceBuffer();
			void createGeometryBuffer();

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
			ref<PipelineLayout> presentPipelineLayout;

			ref<RenderPipeline> visibilityRenderPipeline;
			ref<RenderPipeline> presentRenderPipeline;

			ref<Buffer> indirectBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> geometryBuffer;

			uint32* indirectBufferMappedRange = nullptr;
			Camera* cameraBufferMappedRange = nullptr;
			Instance* instanceBufferMappedRange = nullptr;
	};
}