#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleRenderer : public Renderer {
		public:
			TriangleRenderer();

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