#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleRenderer : public Renderer {
		public:
			TriangleRenderer();

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			struct Vertex {
				svec2<float32> position;
				svec3<float32> color;
			};

		private:
			ref<Buffer> vertexBuffer;
			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;
	};
}