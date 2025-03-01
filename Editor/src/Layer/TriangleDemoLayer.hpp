#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class TriangleDemoLayer : public Layer {
		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<CommandBuffer> commandBuffer;

		public:
			TriangleDemoLayer();

			virtual ~TriangleDemoLayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			virtual void renderImGui() override;

			void onEvent(Event& event) override;
	};
}