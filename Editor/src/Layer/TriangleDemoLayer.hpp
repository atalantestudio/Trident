#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	struct TriangleDemoVertex {
		vec3<float32> position;
		vec3<float32> color;
	};

	class TriangleDemoLayer : public Layer {
		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
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