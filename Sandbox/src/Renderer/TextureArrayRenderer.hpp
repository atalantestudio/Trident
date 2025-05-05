#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureArrayRenderer : public Renderer {
		public:
			TextureArrayRenderer();

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			ref<Image> loadImage(const std::string& imagePath);

			void setupSampler();

			void setupRenderPipeline();

			void setupVertexBuffer();

			void setupInstanceVertexBuffer();

		private:
			struct Vertex {
				vec2<float32> position;
			};

			struct Instance {
				vec2<float32> offset;
				uint32 textureIndex;
			};

		private:
			ref<PipelineLayout> renderPipelineLayout;
			ref<RenderPipeline> renderPipeline;

			ref<Buffer> vertexBuffer;
			ref<Buffer> instanceVertexBuffer;

			ref<Image> baboonImage;
			ref<Image> cameramanImage;
			ref<Image> lennaImage;
			ref<Image> pepperImage;

			ref<ImageView> baboonImageView;
			ref<ImageView> cameramanImageView;
			ref<ImageView> lennaImageView;
			ref<ImageView> pepperImageView;

			ref<Sampler> sampler;
	};
}