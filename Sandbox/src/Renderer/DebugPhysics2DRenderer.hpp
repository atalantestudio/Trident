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

			ref<Buffer> cameraBuffer;
			CameraUniform* cameraMappedRange = nullptr;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> fillRenderPipeline;
			ref<RenderPipeline> lineRenderPipeline;

			uint32 indexCount = 0;

			Scene2D scene;
	};
}