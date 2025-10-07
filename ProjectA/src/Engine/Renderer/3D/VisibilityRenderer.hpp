#pragma once

#include "ProjectA.hpp"

#include "Engine/Build/Clusterizer.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace ProjectA {
	class VisibilityRenderer : public SceneRenderer<ClusteredScene> {
		private:
			struct Camera {
				mat4x4<float32> projectionView;
				float32 nearPlane;
				float32 farPlane;
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

				// Value to subtract from the index offset to get to the start of the geometry used by this instance.
				uint32 indexRollback;
			};

			struct Geometry {
				uint32 vertexOffset;
				uint32 clusterCount;
			};

		private:
			static constexpr uint32 VISIBILITY_SUBPASS_INDEX = 0;
			static constexpr uint32 DEBUG_SUBPASS_INDEX = 1;

		public:
			void build() override;

			void setScene(const ClusteredScene* scene) override;

			void setCamera(const Camera3D* camera) override;

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCameraProjection();

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
			void createDebugDescriptorSetLayout();
			void createVisibilityPipelineLayout();
			void createDebugPipelineLayout();
			void createVisibilityRenderPipeline();
			void createDebugRenderPipeline();
			void createIndirectBuffer();
			void createCameraBuffer();
			void createVertexPositionComponentBuffer();
			void createVertexBuffer();
			void createClusterBuffer();
			void createInstanceBuffer();
			void createGeometryBuffer();

			void allocateDescriptorSets();

			void fillSceneBuffers() const;
			void fillIndirectBuffer() const;
			void fillVertexPositionComponentBuffer(float32* mappedRange) const;
			void fillVertexBuffer(Vertex* mappedRange) const;
			void fillClusterBuffer(Cluster* mappedRange) const;
			void fillInstanceBuffer(Instance* mappedRange) const;
			void fillGeometryBuffer(Geometry* mappedRange) const;

			void updateBufferDescriptorSets();
			void updateDebugDescriptorSet();

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
			ref<DescriptorSetLayout> debugDescriptorSetLayout;

			ref<DescriptorSet> cameraDescriptorSet;
			ref<DescriptorSet> clusterDescriptorSet;
			ref<DescriptorSet> instanceDescriptorSet;
			ref<DescriptorSet> debugDescriptorSet;

			ref<PipelineLayout> visibilityPipelineLayout;
			ref<PipelineLayout> debugPipelineLayout;

			ref<RenderPipeline> visibilityRenderPipeline;
			ref<RenderPipeline> debugRenderPipeline;

			ref<Buffer> indirectBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> vertexPositionComponentBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> clusterBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> geometryBuffer;

			DrawIndirectCommand* indirectMappedRange;
			Camera* cameraMappedRange;
			Instance* instanceMappedRange;
	};
}