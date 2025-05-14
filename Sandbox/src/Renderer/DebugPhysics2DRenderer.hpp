#pragma once

#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DebugPhysics2DRenderer : public Renderer {
		public:
			DebugPhysics2DRenderer();

			// After registering the scene, the vertex buffer cannot be resized.
			void registerScene(const Scene2D& scene);

			// x and y are in the range [0, viewport].
			const vec2<float32>& getNdcMousePosition() const;

			// x and y are in the range [0, viewport].
			void setNdcMousePosition(float32 x, float32 y);

			void zoom(float32 zoomLevelDelta) const;

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

			void resize(float32 width, float32 height) const;

			void writeInstanceTransform(uint32 instanceIndex, const mat3x3<float32>& transform) const;

			void setVertexFeature(uint32 vertexFeatureIndex, const vec2<float32>& vertexFeature);

		private:
			void initializeViewUniformBuffer();
			void initializeCameraUniformBuffer();
			void initializeGridVertexBuffer();
			void initializeGridRenderPipeline();
			void initializeVertexFeatureBuffer();
			void initializeVertexFeatureRenderPipeline();

			void renderGrid(const ref<CommandEncoder>& commandEncoder) const;
			void renderScene(const ref<CommandEncoder>& commandEncoder) const;
			void renderClosestFeatures(const ref<CommandEncoder>& commandEncoder) const;

		private:
			struct Instance {
				mat3x3<float32> world;
				vec4<float32> fillColor;
			};

			struct CameraUniform {
				mat3x3<float32> projection;
				float32 zoomLevel;
			};

			struct ViewUniform {
				svec2<float32> viewport;
			};

		private:
			static constexpr float32 DEFAULT_ZOOM_LEVEL = 64.0f;
			static constexpr float32 MIN_ZOOM_LEVEL = 16.0f;
			static constexpr float32 MAX_ZOOM_LEVEL = 128.0f;
			static constexpr float32 ZOOM_SENSITIVITY = 4.0f;

		private:
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> vertexFeatureBuffer;
			svec2<float32>* vertexFeatureMappedRange = nullptr;
			uint32 vertexFeatureCount = 0;

			ref<Buffer> instanceBuffer;
			Instance* instanceMappedRange = nullptr;

			ref<Buffer> gridVertexBuffer;

			ref<Buffer> cameraUniformBuffer;
			CameraUniform* cameraMappedRange = nullptr;

			ref<Buffer> viewUniformBuffer;
			ViewUniform* viewMappedRange = nullptr;

			ref<PipelineLayout> gridRenderPipelineLayout;
			ref<RenderPipeline> gridRenderPipeline;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> fillRenderPipeline;
			ref<RenderPipeline> strokeRenderPipeline;

			ref<PipelineLayout> featureRenderPipelineLayout;
			ref<RenderPipeline> vertexFeatureRenderPipeline;
			//ref<RenderPipeline> edgeFeatureRenderPipeline;

			uint32 indexCount = 0;
			uint32 gridVertexCount = 0;

			// Mouse position in zoomed NDC coordinates.
			vec2<float32> mousePosition;

			const Scene2D* scene = nullptr;
	};
}