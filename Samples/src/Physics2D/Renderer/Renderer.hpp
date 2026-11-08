// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Renderer.hpp"

#include "Physics2D/Renderer/BuildOptions.hpp"

namespace Trident {
	// Toggle vertex/edge feature rendering.
	// #define SHOW_FEATURES

	// Toggle closest point distance rendering.
	// #define SHOW_DISTANCE

	class Physics2DRenderer : public Renderer {
		private:
			struct Camera {
				mat4x3<float32> projection;
			};

			struct View {
				vec2<float32> viewport;
			};

			struct Vertex {
				vec2<float32> position;
			};

			struct RigidBody {
				mat3x3<float32> world;
				vec4<float32> fillColor;
				vec4<float32> strokeColor;
				float32 depth;
				uint32 selected;
			};

			struct CenterOfMass {
				vec3<float32> position;
			};

			struct Placeholder {
				vec2<float32> offset;
			};

		private:
			static constexpr float32 MIN_ZOOM_LEVEL = 1.0f;
			static constexpr float32 MAX_ZOOM_LEVEL = 64.0f;
			static constexpr float32 DEFAULT_ZOOM_LEVEL = 32.0f;

		public:
			void build(const PhysicsRendererBuildOptions2D& options);

			StaticMemoryDescriptor getStaticMemoryDescriptor() const override;

			void postBuild() override;

			void encodeOneTimeCommands() const;

			inline float32 getZoomLevel() const {
				return cameraMappedRange->projection.m[2].w;
			}

			// void setPlaceholderOffset(vec2<float32> offset) const;

			#if defined(SHOW_DISTANCE)
				// void setClosestPoints(vec2<float32> closestPointA, vec2<float32> closestPointB) const;
			#endif

			#if defined(SHOW_FEATURES)
				// void registerFeature(const Feature2D& feature);
			#endif

			// void selectEntity(Entity entity);

			void setTransform(uint32 hullIndex, const Transform2D& transform);

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCameraProjection() const;

			void zoom(float32 delta);

			void resetCounters();

		private:
			void build() override;

			ref<RenderPass> createRenderPass() const;

			ref<Image> createColorImage() const;
			ref<Image> createDepthStencilImage() const;

			ref<ImageView> createColorImageView() const;
			ref<ImageView> createDepthStencilImageView() const;

			void createFramebuffers();

			ref<Buffer> createVertexBuffer(uint32 vertexCount) const;
			ref<Buffer> createTransformBuffer(uint32 transformCount) const;
			ref<Buffer> createIndexBuffer(uint32 indexCount) const;
			ref<Buffer> createCameraBuffer() const;
			void createViewBuffer();
			// void createPlaceholderBuffer();

			#if defined(SHOW_DISTANCE)
				void createClosestPointBuffer();
			#endif

			#if defined(SHOW_FEATURES)
				void createVertexFeatureBuffer();
				void createEdgeFeatureBuffer();
			#endif

			void createCameraViewDescriptorSetLayout();
			// void createPlaceholderDescriptorSetLayout();
			void createCameraViewPipelineLayout();
			// void createPlaceholderPipelineLayout();

			ref<GraphicsPipeline> createGridPipeline() const;
			ref<GraphicsPipeline> createCirclePipeline() const;
			ref<GraphicsPipeline> createShapeFillPipeline() const;
			ref<GraphicsPipeline> createShapeStrokePipeline() const;
			// ref<GraphicsPipeline> createCenterOfMassPipeline() const;
			// ref<GraphicsPipeline> createRigidBodyPlaceholderFillPipeline() const;
			// ref<GraphicsPipeline> createRigidBodyPlaceholderStrokePipeline() const;
			// ref<GraphicsPipeline> createRigidBodyPlaceholderCenterOfMassPipeline() const;

			#if defined(SHOW_DISTANCE)
				// ref<GraphicsPipeline> createDistancePipeline() const;
			#endif

			#if defined(SHOW_FEATURES)
				// ref<GraphicsPipeline> createVertexFeaturePipeline() const;
				// ref<GraphicsPipeline> createEdgeFeaturePipeline() const;
			#endif

			void updateCameraViewDescriptorSet() const;
			// void updatePlaceholderDescriptorSet() const;
			// void updateCentersOfMass() const;
			void updateViewport() const;

			#if defined(SHOW_DISTANCE)
				void renderDistance(ref<CommandEncoder> commandEncoder) const;
			#endif

			#if defined(SHOW_FEATURES)
				void renderFeatures(ref<CommandEncoder> commandEncoder) const;
			#endif

		public:
			vec2<float32> cameraPosition = {0.0_m, 0.0_m};

			// Transform mapped ranges per shape type.
			sequence<Transform2D*> transformMappedRanges;

		private:
			ref<RenderPass> renderPass;

			ref<Image> colorImage;
			ref<Image> depthStencilImage;

			ref<ImageView> colorImageView;
			ref<ImageView> depthStencilImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraViewDescriptorSetLayout;
			// ref<DescriptorSetLayout> placeholderDescriptorSetLayout;

			ref<DescriptorSet> cameraViewDescriptorSet;
			// ref<DescriptorSet> placeholderDescriptorSet;

			ref<PipelineLayout> cameraViewPipelineLayout;
			// ref<PipelineLayout> placeholderPipelineLayout;

			ref<GraphicsPipeline> gridPipeline;
			ref<GraphicsPipeline> circlePipeline;
			ref<GraphicsPipeline> shapeFillPipeline;
			ref<GraphicsPipeline> shapeStrokePipeline;
			// ref<GraphicsPipeline> centerOfMassPipeline;
			// ref<GraphicsPipeline> rigidBodyPlaceholderFillPipeline;
			// ref<GraphicsPipeline> rigidBodyPlaceholderStrokePipeline;
			// ref<GraphicsPipeline> rigidBodyPlaceholderCenterOfMassPipeline;

			#if defined(SHOW_DISTANCE)
				ref<GraphicsPipeline> distancePipeline;
			#endif

			#if defined(SHOW_FEATURES)
				ref<GraphicsPipeline> vertexFeaturePipeline;
				ref<GraphicsPipeline> edgeFeaturePipeline;
			#endif

			ref<Buffer> vertexBuffer;
			ref<Buffer> transformBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> cameraBuffer;
			ref<Buffer> viewBuffer;
			ref<Buffer> placeholderBuffer;

			#if defined(SHOW_DISTANCE)
				ref<Buffer> closestPointBuffer;
			#endif

			#if defined(SHOW_FEATURES)
				ref<Buffer> vertexFeatureBuffer;
				ref<Buffer> edgeFeatureBuffer;
			#endif

			ref<Buffer> rigidBodyBuffer;
			ref<Buffer> centerOfMassBuffer;

			Camera* cameraMappedRange = nullptr;
			View* viewMappedRange = nullptr;
			Placeholder* placeholderMappedRange = nullptr;

			#if defined(SHOW_DISTANCE)
				Vertex* closestPointMappedRange = nullptr;
			#endif

			#if defined(SHOW_FEATURES)
				Vertex* vertexFeatureMappedRange = nullptr;
				Vertex* edgeFeatureMappedRange = nullptr;
			#endif

			RigidBody* rigidBodyMappedRange = nullptr;
			CenterOfMass* centerOfMassMappedRange = nullptr;

			#if defined(SHOW_FEATURES)
				uint32 vertexFeatureCount = 0;
				uint32 edgeFeatureCount = 0;
			#endif

			BufferView<Transform2D> transformBufferView;
			BufferView<Camera> cameraView;

			ImageSampleCount sampleCount;
			uint32 maxCircleTransformCount;
			uint32 circleTransformCount;
			sequence<PhysicsRendererBuildHull2D> hulls;
			uint64 lineStripIndexOffset;
	};
}