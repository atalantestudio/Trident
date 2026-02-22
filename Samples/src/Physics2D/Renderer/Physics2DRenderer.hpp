// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	// Toggle vertex/edge feature rendering.
	// #define SHOW_FEATURES

	// Toggle closest point distance rendering.
	// #define SHOW_DISTANCE

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

			struct Placeholder {
				svec2<float32> offset;
			};

		private:
			static constexpr uint8 SAMPLE_COUNT = 4;
			static constexpr float32 MIN_ZOOM_LEVEL = 1.0f;
			static constexpr float32 MAX_ZOOM_LEVEL = 64.0f;
			static constexpr float32 DEFAULT_ZOOM_LEVEL = 32.0f;

		public:
			void build() override;

			// `x` and `y` are in the [0, viewport] range.
			const vec2<float32>& getNdcMousePosition() const;

			void setScene(const Scene2D& scene);

			#if defined(SHOW_DISTANCE)
				void setClosestPoints(const vec2<float32>& closestPointA, const vec2<float32>& closestPointB) const;
			#endif

			void setInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const;

			// `x` and `y` are in the [0, viewport] range.
			void setMousePosition(float32 x, float32 y);

			void setPlaceholderOffset(const vec2<float32>& offset) const;

			#if defined(SHOW_FEATURES)
				void registerFeature(const Feature2D& feature);
			#endif

			void selectEntity(Entity entity);

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void zoom(float32 zoomLevelDelta);

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
			void createPlaceholderBuffer();

			#if defined(SHOW_DISTANCE)
				void createClosestPointBuffer();
			#endif

			#if defined(SHOW_FEATURES)
				void createVertexFeatureBuffer();
				void createEdgeFeatureBuffer();
			#endif

			void createCameraViewDescriptorSetLayout();
			void createPlaceholderDescriptorSetLayout();
			void createCameraViewPipelineLayout();
			void createPlaceholderPipelineLayout();
			void createGridGraphicsPipeline();
			void createFillGraphicsPipeline();
			void createStrokeGraphicsPipeline();
			void createCenterOfMassGraphicsPipeline();
			void createPlaceholderFillGraphicsPipeline();
			void createPlaceholderStrokeGraphicsPipeline();
			void createPlaceholderCenterOfMassGraphicsPipeline();

			#if defined(SHOW_DISTANCE)
				void createDistanceGraphicsPipeline();
			#endif

			#if defined(SHOW_FEATURES)
				void createVertexFeatureGraphicsPipeline();
				void createEdgeFeatureGraphicsPipeline();
			#endif

			void allocateDescriptorSets();

			void copyBuffers(ref<Buffer> stagingBuffer) const;

			void updateCameraViewDescriptorSet() const;
			void updatePlaceholderDescriptorSet() const;
			void updateCentersOfMass() const;
			void updateCameraProjection() const;
			void updateViewport() const;

			void renderGrid(ref<CommandEncoder> commandEncoder) const;
			void renderScene(ref<CommandEncoder> commandEncoder) const;
			void renderSelectedInstancePlaceholder(ref<CommandEncoder> commandEncoder) const;

			#if defined(SHOW_DISTANCE)
				void renderDistance(ref<CommandEncoder> commandEncoder) const;
			#endif

			#if defined(SHOW_FEATURES)
				void renderFeatures(ref<CommandEncoder> commandEncoder) const;
			#endif

		public:
			LayerInput* layerInput;

		private:
			const Scene2D* scene = nullptr;

			ref<RenderPass> renderPass;

			ref<Image> depthStencilImage;

			ref<ImageView> depthStencilImageView;

			sequence<ref<Framebuffer>> framebuffers;

			ref<DescriptorSetLayout> cameraViewDescriptorSetLayout;
			ref<DescriptorSetLayout> placeholderDescriptorSetLayout;

			ref<DescriptorSet> cameraViewDescriptorSet;
			ref<DescriptorSet> placeholderDescriptorSet;

			ref<PipelineLayout> cameraViewPipelineLayout;
			ref<PipelineLayout> placeholderPipelineLayout;

			ref<GraphicsPipeline> gridGraphicsPipeline;
			ref<GraphicsPipeline> fillGraphicsPipeline;
			ref<GraphicsPipeline> strokeGraphicsPipeline;
			ref<GraphicsPipeline> centerOfMassGraphicsPipeline;
			ref<GraphicsPipeline> placeholderFillGraphicsPipeline;
			ref<GraphicsPipeline> placeholderStrokeGraphicsPipeline;
			ref<GraphicsPipeline> placeholderCenterOfMassGraphicsPipeline;

			#if defined(SHOW_DISTANCE)
				ref<GraphicsPipeline> distanceGraphicsPipeline;
			#endif

			#if defined(SHOW_FEATURES)
				ref<GraphicsPipeline> vertexFeatureGraphicsPipeline;
				ref<GraphicsPipeline> edgeFeatureGraphicsPipeline;
			#endif

			ref<Buffer2<Camera>> cameraBuffer;
			ref<Buffer2<View>> viewBuffer;
			ref<Buffer2<Placeholder>> placeholderBuffer;

			#if defined(SHOW_DISTANCE)
				ref<Buffer> closestPointBuffer;
			#endif

			#if defined(SHOW_FEATURES)
				ref<Buffer> vertexFeatureBuffer;
				ref<Buffer> edgeFeatureBuffer;
			#endif

			ref<Buffer2<Vertex>> vertexBuffer;
			ref<Buffer2<Instance>> instanceBuffer;
			ref<Buffer2<CenterOfMass>> centerOfMassBuffer;
			ref<Buffer> indexBuffer;

			Camera* cameraBufferMappedRange = nullptr;
			View* viewBufferMappedRange = nullptr;
			Placeholder* placeholderBufferMappedRange = nullptr;

			#if defined(SHOW_DISTANCE)
				Vertex* closestPointBufferMappedRange = nullptr;
			#endif

			#if defined(SHOW_FEATURES)
				Vertex* vertexFeatureBufferMappedRange = nullptr;
				Vertex* edgeFeatureBufferMappedRange = nullptr;
			#endif

			Instance* instanceBufferMappedRange = nullptr;
			CenterOfMass* centerOfMassBufferMappedRange = nullptr;

			vec2<float32> mousePosition;
			vec2<float32> ndcMousePosition;

			uint32 selectedInstanceIndex = max<uint32>();

			#if defined(SHOW_FEATURES)
				uint32 vertexFeatureCount = 0;
				uint32 edgeFeatureCount = 0;
			#endif
	};
}