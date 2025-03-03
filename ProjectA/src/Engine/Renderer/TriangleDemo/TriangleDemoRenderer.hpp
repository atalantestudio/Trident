#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleDemoRenderer : public Renderer {
		public:
			TriangleDemoRenderer();

			void render() const;

		private:
			struct Vertex {
				vec3<float32> position;
				vec3<float32> color;
			};

		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<CommandBuffer> commandBuffer;
	};
}