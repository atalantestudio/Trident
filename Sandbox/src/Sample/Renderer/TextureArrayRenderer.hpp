#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureArrayRenderer : public Renderer {
		public:
			TextureArrayRenderer();

			void render(const ref<CommandEncoder>& commandEncoder) const final override;

		private:
			ref<Image> loadImage(const std::string& imagePath);

			void initializeSampler();
			void initializeRenderPipeline();
			void initializeVertexBuffer();
			void initializeInstanceVertexBuffer();

		private:
			struct Instance {
				// TODO: Remove padding.
				vec4<float32> offset;

				uint32 textureIndex;
			};

		private:
			ref<Swapchain> swapchain;

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