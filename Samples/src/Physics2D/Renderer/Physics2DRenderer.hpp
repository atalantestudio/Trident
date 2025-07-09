#pragma once

#include "Engine/API/Physics/ClosestFeature2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class Physics2DRenderer : public Renderer {
		public:
			void build() override;

			// After registering the scene, the vertex buffer cannot be resized.
			void registerScene(const Scene2D& scene);

			// x and y are in the range [0, viewport].
			const vec2<float32>& getNdcMousePosition() const;

			// x and y are in the range [0, viewport].
			void setMousePosition(float32 x, float32 y);

			void zoom(float32 zoomLevelDelta);

			void setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const;

			void render(const ref<CommandEncoder>& commandEncoder) const override;

			void selectEntity(Entity entity);

			void setPlaceholderOffset(const vec2<float32>& offset) const;

			void resize(WindowResizeEvent& event) override;

			void writeInstanceTransform(Entity entity, const mat3x3<float32>& transform) const;

			void clearClosestFeatureCounters();

			void registerClosestFeature(const ClosestFeature2D& closestFeature);

		private:
			void recreateFramebuffers();

			void initializeViewUniformBuffer();
			void initializeCameraUniformBuffer();
			void initializeGridVertexBuffer();
			void initializeGridRenderPipeline();
			void initializeCenterOfMassBuffer();
			void initializeCenterOfMassRenderPipeline();
			void initializeClosestPointBuffer();
			void initializeDistanceRenderPipeline();
			void initializeVertexFeatureBuffer();
			void initializeEdgeVertexFeatureBuffer();
			void initializeFeaturePipelineLayout();
			void initializeVertexFeatureRenderPipeline();
			void initializeEdgeFeatureRenderPipeline();

			void updateCentersOfMass() const;

			void renderGrid(const ref<CommandEncoder>& commandEncoder) const;
			void renderScene(const ref<CommandEncoder>& commandEncoder) const;
			void renderDistance(const ref<CommandEncoder>& commandEncoder) const;
			void renderClosestFeatures(const ref<CommandEncoder>& commandEncoder) const;
			void renderSelectedEntityPlaceholder(const ref<CommandEncoder>& commandEncoder) const;

		private:
			struct Instance {
				mat3x3<float32> world;
				vec4<float32> fillColor;

				// TODO: Remove padding.
				vec4<float32> z;

				uint32 selected = 0;
			};

			struct CameraUniform {
				mat3x3<float32> projection;
				float32 zoomLevel;
			};

			struct ViewUniform {
				svec2<float32> viewport;
			};

			struct PlaceholderUniform {
				svec2<float32> offset;
			};

		private:
			static constexpr uint8 SAMPLE_COUNT = 4;
			static constexpr float32 MIN_ZOOM_LEVEL = 1.0f;
			static constexpr float32 MAX_ZOOM_LEVEL = 64.0f;
			static constexpr float32 DEFAULT_ZOOM_LEVEL = MAX_ZOOM_LEVEL;

		private:
			ref<Subpass> subpass;
			ref<RenderPass> renderPass;

			sequence<ref<Framebuffer>> framebuffers;

			ref<Image> colorImage;
			ref<ImageView> colorImageView;

			ref<Image> depthStencilImage;
			ref<ImageView> depthStencilImageView;

			ref<Buffer> gridVertexBuffer;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> centerOfMassBuffer;
			vec3<float32>* centerOfMassMappedRange = nullptr;

			ref<Buffer> vertexFeatureBuffer;
			svec2<float32>* vertexFeatureMappedRange = nullptr;
			uint32 vertexFeatureCount = 0;

			ref<Buffer> edgeVertexFeatureBuffer;
			svec2<float32>* edgeVertexFeatureMappedRange = nullptr;
			uint32 edgeVertexFeatureCount = 0;

			ref<Buffer> instanceBuffer;
			Instance* instanceMappedRange = nullptr;

			ref<Buffer> closestPointBuffer;
			svec2<float32>* closestPointMappedRange = nullptr;

			ref<Buffer> cameraUniformBuffer;
			CameraUniform* cameraMappedRange = nullptr;

			ref<Buffer> viewUniformBuffer;
			ViewUniform* viewMappedRange = nullptr;

			ref<PipelineLayout> gridPipelineLayout;
			ref<RenderPipeline> gridRenderPipeline;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> fillRenderPipeline;
			ref<RenderPipeline> strokeRenderPipeline;

			ref<Buffer> placeholderUniformBuffer;
			PlaceholderUniform* placeholderMappedRange = nullptr;

			ref<PipelineLayout> placeholderFillPipelineLayout;
			ref<RenderPipeline> placeholderFillRenderPipeline;

			ref<PipelineLayout> placeholderStrokePipelineLayout;
			ref<RenderPipeline> placeholderStrokeRenderPipeline;

			// The center of mass pipeline uses the same layout as the closest feature pipelines.
			ref<RenderPipeline> centerOfMassRenderPipeline;

			ref<RenderPipeline> placeholderCenterOfMassRenderPipeline;

			ref<PipelineLayout> distancePipelineLayout;
			ref<RenderPipeline> distanceRenderPipeline;

			ref<PipelineLayout> featurePipelineLayout;
			ref<RenderPipeline> vertexFeatureRenderPipeline;
			ref<RenderPipeline> edgeFeatureRenderPipeline;

			uint32 indexCount = 0;
			uint32 gridVertexCount = 0;

			vec2<float32> mousePosition;
			vec2<float32> ndcMousePosition;

			Entity selectedEntity = max<uint32>();

			const Scene2D* scene = nullptr;
	};
}