#pragma once

#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class Physics2DRenderer : public Renderer {
		private:
			struct Camera {
				mat3x3<float32> projection;
				float32 zoomLevel;
			};

			struct View {
				svec2<float32> viewport;
			};

			struct Vertex {
				svec2<float32> position;
			};

			struct Instance {
				mat3x3<float32> world;
				vec4<float32> fillColor;
				vec4<float32> strokeColor;
				float32 depth;
				uint32 selected = 0;
			};

			struct CenterOfMass {
				vec3<float32> position;
			};

			struct ClosestPoint {
				svec2<float32> position;
			};

			struct Placeholder {
				svec2<float32> offset;
			};

		private:
			static constexpr bool ENABLE_DISTANCE = false;
			static constexpr uint8 SAMPLE_COUNT = 4;
			static constexpr float32 MIN_ZOOM_LEVEL = 1.0f;
			static constexpr float32 MAX_ZOOM_LEVEL = 64.0f;
			static constexpr float32 DEFAULT_ZOOM_LEVEL = 32.0f;

		public:
			void build() override;

			// x and y are in the range [0, viewport].
			const vec2<float32>& getNdcMousePosition() const;

			void setScene(const Scene2D& scene);
			void setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const;
			void setInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const;

			// x and y are in the range [0, viewport].
			void setMousePosition(float32 x, float32 y);

			// void setPlaceholderOffset(const vec2<float32>& offset) const;

			void selectEntity(Entity entity);

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void zoom(float32 zoomLevelDelta);

			// void registerFeature(const Feature2D& feature);

			void clearRegisteredData();

		private:
			void setupRenderPass();

			void recreateMultisampleImage();
			void recreateMultisampleImageView();
			void recreateDepthStencilImage();
			void recreateDepthStencilImageView();
			void recreateFramebuffers();
			void recreateVertexBuffer(uint64 size);
			void recreateInstanceBuffer(uint64 size);
			void recreateCenterOfMassBuffer(uint64 size);
			void recreateIndexBuffer(uint64 size);

			void createCameraBuffer();
			void createViewBuffer();
			void createClosestPointBuffer();
			void createCameraViewDescriptorSetLayout();
			void createCameraViewPipelineLayout();
			void createGridRenderPipeline();
			void createFillRenderPipeline();
			void createStrokeRenderPipeline();
			void createCenterOfMassRenderPipeline();
			void createDistanceRenderPipeline();
			/* void createCenterOfMassBuffer();
			void createVertexFeatureBuffer();
			void createEdgeVertexFeatureBuffer();
			void createFeaturePipelineLayout();
			void createVertexFeatureRenderPipeline();
			void createEdgeFeatureRenderPipeline(); */

			void allocateDescriptorSets();

			void copyBuffers(const ref<Buffer>& stagingBuffer) const;

			void updateCameraViewDescriptorSet() const;
			void updateCentersOfMass() const;
			void updateCameraProjection() const;
			void updateViewport() const;

			void renderGrid(const ref<CommandEncoder>& commandEncoder) const;
			void renderScene(const ref<CommandEncoder>& commandEncoder) const;
			void renderDistance(const ref<CommandEncoder>& commandEncoder) const;
			// void renderFeatures(const ref<CommandEncoder>& commandEncoder) const;
			// void renderSelectedInstancePlaceholder(const ref<CommandEncoder>& commandEncoder) const;

		private:
			const Scene2D* scene = nullptr;

			ref<RenderPass> renderPass;

			ref<Image> multisampleImage;
			ref<Image> depthStencilImage;

			ref<ImageView> multisampleImageView;
			ref<ImageView> depthStencilImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraViewDescriptorSetLayout;

			ref<DescriptorSet> cameraViewDescriptorSet;

			ref<PipelineLayout> cameraViewPipelineLayout;

			ref<RenderPipeline> gridRenderPipeline;
			ref<RenderPipeline> fillRenderPipeline;
			ref<RenderPipeline> strokeRenderPipeline;
			ref<RenderPipeline> centerOfMassRenderPipeline;
			ref<RenderPipeline> distanceRenderPipeline;

			ref<Buffer> cameraBuffer;
			ref<Buffer> viewBuffer;
			ref<Buffer> closestPointBuffer;
			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceBuffer;
			ref<Buffer> centerOfMassBuffer;
			ref<Buffer> indexBuffer;

			Camera* cameraBufferMappedRange = nullptr;
			View* viewBufferMappedRange = nullptr;
			ClosestPoint* closestPointBufferMappedRange = nullptr;
			Instance* instanceBufferMappedRange = nullptr;
			CenterOfMass* centerOfMassBufferMappedRange = nullptr;

			vec2<float32> mousePosition;
			vec2<float32> ndcMousePosition;

			uint32 selectedInstanceIndex = max<uint32>();

			/////

			ref<Buffer> vertexFeatureBuffer;
			svec2<float32>* vertexFeatureMappedRange = nullptr;
			uint32 vertexFeatureCount = 0;
			ref<Buffer> edgeVertexFeatureBuffer;
			svec2<float32>* edgeVertexFeatureMappedRange = nullptr;
			uint32 edgeVertexFeatureCount = 0;
			ref<Buffer> placeholderBuffer;
			Placeholder* placeholderBufferMappedRange = nullptr;
			ref<PipelineLayout> placeholderFillPipelineLayout;
			ref<RenderPipeline> placeholderFillRenderPipeline;
			ref<PipelineLayout> placeholderStrokePipelineLayout;
			ref<RenderPipeline> placeholderStrokeRenderPipeline;
			ref<RenderPipeline> placeholderCenterOfMassRenderPipeline;
			ref<PipelineLayout> featurePipelineLayout;
			ref<RenderPipeline> vertexFeatureRenderPipeline;
			ref<RenderPipeline> edgeFeatureRenderPipeline;
	};
}