#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class GUIRenderer : public Renderer {
		public:
			GUIRenderer();

			void render() const override;

		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			//ref<Buffer> indexBuffer;
			ref<CommandEncoder> commandEncoder;
	};
}