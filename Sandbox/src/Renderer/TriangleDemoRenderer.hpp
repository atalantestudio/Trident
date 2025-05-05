#pragma once

#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleDemoRenderer : public Renderer {
		public:
			TriangleDemoRenderer();

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			void setupVertexBuffer();
			void setupRenderPipeline();

		private:
			struct Vertex {
				vec2<float32> position;
				vec3<float32> color;
			};

		private:
			ref<Buffer> vertexBuffer;
			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;
	};
}