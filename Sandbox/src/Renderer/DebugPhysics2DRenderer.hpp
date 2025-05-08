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

			void updateCamera(float32 width, float32 height) const;

		private:
			void initializeLineVertexBuffer();

			void initializeLineColorUniformBuffer();

			void initializeLineRenderPipeline();

			void renderGrid(const ref<CommandEncoder>& commandEncoder) const;

			void renderScene(const ref<CommandEncoder>& commandEncoder) const;

		private:
			struct Instance {
				vec4<float32> fillColor;
			};

			struct CameraUniform {
				mat3x3<float32> projection;
				float32 zoomLevel;
			};

		private:
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> instanceBuffer;

			ref<Buffer> lineVertexBuffer;

			ref<Buffer> lineColorUniformBuffer;
			vec4<float32>* lineColorMappedRange = nullptr;

			ref<Buffer> cameraBuffer;
			CameraUniform* cameraMappedRange = nullptr;

			ref<PipelineLayout> lineRenderPipelineLayout;
			ref<RenderPipeline> lineRenderPipeline;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> fillRenderPipeline;
			ref<RenderPipeline> strokeRenderPipeline;

			uint32 indexCount = 0;
			uint32 gridVertexCount = 0;

			Scene2D scene;
	};
}