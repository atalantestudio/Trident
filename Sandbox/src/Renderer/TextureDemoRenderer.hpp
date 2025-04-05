#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureDemoRenderer : public Renderer {
		public:
			TextureDemoRenderer();

			void build(const ref<Image>& image);

			void render(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

			void createImageSampler();
			void createVertexBuffer();
			void createIndexBuffer();
			void createPipeline();

		private:
			struct Vertex {
				vec2<float32> position;
				vec2<float32> uv;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Buffer> cameraUniformBuffer;
			ref<Image> image;
			ref<ImageView> imageView;
			ref<Sampler> sampler;
	};
}