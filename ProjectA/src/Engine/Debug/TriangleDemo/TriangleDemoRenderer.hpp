#pragma once

#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	class TriangleDemoRenderer : public Renderer {
		public:
			TriangleDemoRenderer();

			void render() override;

			void onEvent(Event& event) override;

			bool onWindowResize(const WindowResizeEvent& event);

		private:
			struct Vertex {
				vec2<float32> position;
				vec3<float32> color;
			};

		private:
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

		private:
			ref<PipelineLayout> pipelineLayout;
			ref<RenderPipeline> renderPipeline;
			ref<Buffer> vertexBuffer;
			ref<Buffer> indexBuffer;
			ref<CommandEncoder> commandEncoder;
			chain<ref<Semaphore>> availableImageSemaphores;
			chain<ref<Semaphore>> finishedRenderSemaphores;
			chain<ref<Fence>> flightFences;
			uint32 frameIndex = 0;
	};
}