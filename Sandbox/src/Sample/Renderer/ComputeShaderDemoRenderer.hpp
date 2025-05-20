#pragma once

#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ComputeShaderDemoRenderer : public Renderer {
		public:
			ComputeShaderDemoRenderer();

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			ref<Swapchain> swapchain;

			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;

			// This image is written to by the compute shader.
			ref<Image> image;
			ref<ImageView> imageView;
			ref<Sampler> sampler;
	};
}