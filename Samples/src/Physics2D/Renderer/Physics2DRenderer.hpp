// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Feature2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	#define PHYSICS_2D_RENDERER_SAMPLE_COUNT 1

	// Toggle vertex/edge feature rendering.
	#define SHOW_FEATURES

	// Toggle closest point distance rendering.
	#define SHOW_DISTANCE

	// TODO(physics/2d/renderer): Deprecate LayerInput (replaced by layer groups).
	class Physics2DRenderer : public Renderer {
		private:
			struct Camera {
				mat3x3<float32> projection;
				float32 zoomLevel;
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
			void build() override;

			inline float32 getZoomLevel() const {
				return cameraMappedRange->zoomLevel;
			}

			void setScene(const Scene2D* scene);
			void setWorld(Entity entity, const mat3x3<float32>& world) const;
			void setPlaceholderOffset(vec2<float32> offset) const;

			#if defined(SHOW_DISTANCE)
				void setClosestPoints(vec2<float32> closestPointA, vec2<float32> closestPointB) const;
			#endif

			#if defined(SHOW_FEATURES)
				void registerFeature(const Feature2D& feature);
			#endif

			void selectEntity(Entity entity);

			void render(ref<CommandEncoder> commandEncoder) const override;

			void resize(WindowResizeEvent& event) override;

			void updateCameraProjection() const;

			void zoom(float32 delta);

			void clearRegisteredData();

		private:
			void setupRenderPass();

			void recreateMultisampleImage();
			void recreateMultisampleImageView();
			void recreateDepthStencilImage();
			void recreateDepthStencilImageView();
			void recreateFramebuffers();

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

			ref<GraphicsPipeline> createGridPipeline() const;
			ref<GraphicsPipeline> createRigidBodyFillPipeline() const;
			ref<GraphicsPipeline> createRigidBodyStrokePipeline() const;
			ref<GraphicsPipeline> createCenterOfMassPipeline() const;
			ref<GraphicsPipeline> createRigidBodyPlaceholderFillPipeline() const;
			ref<GraphicsPipeline> createRigidBodyPlaceholderStrokePipeline() const;
			ref<GraphicsPipeline> createRigidBodyPlaceholderCenterOfMassPipeline() const;

			#if defined(SHOW_DISTANCE)
				ref<GraphicsPipeline> createDistancePipeline() const;
			#endif

			#if defined(SHOW_FEATURES)
				ref<GraphicsPipeline> createVertexFeaturePipeline() const;
				ref<GraphicsPipeline> createEdgeFeaturePipeline() const;
			#endif

			void allocateDescriptorSets();

			void updateCameraViewDescriptorSet() const;
			void updatePlaceholderDescriptorSet() const;
			void updateCentersOfMass() const;
			void updateViewport() const;

			#if defined(SHOW_DISTANCE)
				void renderDistance(ref<CommandEncoder> commandEncoder) const;
			#endif

			#if defined(SHOW_FEATURES)
				void renderFeatures(ref<CommandEncoder> commandEncoder) const;
			#endif

		public:
			LayerInput* layerInput;

			vec2<float32> cameraPosition = {0.0_m, 0.0_m};

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

			ref<GraphicsPipeline> gridPipeline;
			ref<GraphicsPipeline> rigidBodyFillPipeline;
			ref<GraphicsPipeline> rigidBodyStrokePipeline;
			ref<GraphicsPipeline> centerOfMassPipeline;
			ref<GraphicsPipeline> rigidBodyPlaceholderFillPipeline;
			ref<GraphicsPipeline> rigidBodyPlaceholderStrokePipeline;
			ref<GraphicsPipeline> rigidBodyPlaceholderCenterOfMassPipeline;

			#if defined(SHOW_DISTANCE)
				ref<GraphicsPipeline> distancePipeline;
			#endif

			#if defined(SHOW_FEATURES)
				ref<GraphicsPipeline> vertexFeaturePipeline;
				ref<GraphicsPipeline> edgeFeaturePipeline;
			#endif

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

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
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

			Entity selectedEntity = max<Entity>();

			#if defined(SHOW_FEATURES)
				uint32 vertexFeatureCount = 0;
				uint32 edgeFeatureCount = 0;
			#endif
	};
}