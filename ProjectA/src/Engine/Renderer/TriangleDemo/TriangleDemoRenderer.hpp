#pragma once

#include "Engine/Acceleration/seq.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleDemoRenderer : public Renderer {
		public:
			TriangleDemoRenderer();

			void render() override;

		private:
			struct Vertex {
				vec3<float32> position;
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
			ref<CommandEncoder> commandEncoder;
			seq<ref<Semaphore>> availableImageSemaphores;
			seq<ref<Semaphore>> finishedRenderSemaphores;
			seq<ref<Fence>> flightFences;

			uint32 frameIndex = 0;
	};
}