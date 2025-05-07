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
			struct CameraUniform {
				mat3x3<float32> projection;
				float32 zoomLevel;
			};

		private:
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			ref<Buffer> cameraBuffer;
			CameraUniform* cameraMappedRange = nullptr;

			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			uint32 indexCount = 0;
	};
}