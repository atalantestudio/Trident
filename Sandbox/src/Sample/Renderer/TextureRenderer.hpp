#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureRenderer : public Renderer {
		public:
			TextureRenderer();

			void build(const ref<Image>& image);

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

			void createImageSampler();
			void createVertexBuffer();
			void createIndexBuffer();
			void createPipeline();

		private:
			struct Vertex {
				svec2<float32> position;
				svec2<float32> uv;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> cameraUniformBuffer;

			// Keep a reference to the original image to avoid getting an invalid image view while rendering.
			ref<Image> image;

			ref<ImageView> imageView;
			ref<Sampler> sampler;
	};
}