#pragma once

#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class DebugPhysics2DRenderer : public Renderer {
		public:
			// After registering the scene, the vertex buffer cannot be resized.
			void registerScene(const Scene2D& scene);

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			ref<Buffer> vertexBuffer;
			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;
	};
}