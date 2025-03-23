#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TextureDemoRenderer : public Renderer {
		public:
			TextureDemoRenderer();

			void render() override;

			void onEvent(Event& event) override;

		private:
			struct Vertex {
				vec2<float32> position;
				vec2<float32> uv;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<Texture> texture;
			ref<TextureView> textureView;
			ref<Sampler> sampler;
			ref<CommandEncoder> commandEncoder;
			chain<ref<Semaphore>> availableImageSemaphores;
			chain<ref<Semaphore>> finishedRenderSemaphores;
			chain<ref<Fence>> flightFences;
			uint32 frameIndex = 0;
	};
}