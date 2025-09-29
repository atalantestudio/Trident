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

				recreateFramebuffers();

				createCameraBuffer();
			}

			void setScene(const ClusteredScene* scene) override {
				Renderer3D::setScene(scene);

				createVertexPositionComponentBuffer();
				createVertexBuffer();
				createClusterBuffer();
				createInstanceBuffer();
				createGeometryBuffer();

				createVisibilityPipelineLayout();
				createVisibilityRenderPipeline();

				createPresentPipelineLayout();
				createPresentRenderPipeline();

				updateSceneBuffers();
			}

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void resize(WindowResizeEvent& event) override {
				updateVisibilityImage();
				updateVisibilityImageView();

				recreateFramebuffers();

				updateCamera();
			}

			void updateCamera();

			void updateInstanceTransform(Entity entity, const mat4x4<float32>& transform);

		private:
			void updateRenderPass();
			void updateVisibilityImage();
			void updateVisibilityImageView();
			void updateSceneBuffers();

			void recreateFramebuffers();

			void createCameraBuffer();
			void createVertexPositionComponentBuffer();
			void createVertexBuffer();
			void createClusterBuffer();
			void createInstanceBuffer();
			void createGeometryBuffer();
			void createVisibilityPipelineLayout();
			void createVisibilityRenderPipeline();
			void createPresentPipelineLayout();
			void createPresentRenderPipeline();

		private:
			ref<RenderPass> renderPass;

			ref<Image> visibilityImage;
			ref<ImageView> visibilityImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> geometryBuffer;

			Camera* cameraBufferMappedRange = nullptr;
			Instance* instanceBufferMappedRange = nullptr;

			ref<PipelineLayout> visibilityPipelineLayout;
			ref<RenderPipeline> visibilityRenderPipeline;

			ref<PipelineLayout> presentPipelineLayout;
			ref<RenderPipeline> presentRenderPipeline;
	};
}