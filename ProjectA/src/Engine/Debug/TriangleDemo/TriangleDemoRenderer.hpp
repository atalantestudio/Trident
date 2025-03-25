#pragma once

#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleDemoRenderer : public Renderer {
		public:
			TriangleDemoRenderer();

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

		private:
			struct Vertex {
				vec2<float32> position;
				vec3<float32> color;
			};

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
	};
}