#pragma once

#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DebugPhysics2DRenderer : public Renderer {
		public:
			DebugPhysics2DRenderer();

			// After registering the scene, the vertex buffer cannot be resized.
			void registerScene(const Scene2D& scene);

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			void initializeViewUniformBuffer();

			void initializeCameraUniformBuffer();

			void initializeGridVertexBuffer();

			void initializeGridRenderPipeline();

			void renderGrid(const ref<CommandEncoder>& commandEncoder) const;

			void renderScene(const ref<CommandEncoder>& commandEncoder) const;

			void updateCameraUniform(float32 width, float32 height) const;

			void updateViewUniform(float32 width, float32 height) const;

		private:
			struct Instance {
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
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> instanceBuffer;

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

			uint32 indexCount = 0;
			uint32 gridVertexCount = 0;

			Scene2D scene;
	};
}