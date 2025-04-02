#pragma once

#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class ComputeShaderDemoRenderer : public Renderer {
		public:
			ComputeShaderDemoRenderer();

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

		private:
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