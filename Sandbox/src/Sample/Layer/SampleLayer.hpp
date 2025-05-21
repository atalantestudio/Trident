#pragma once

#include "Engine/Layer/Layer.hpp"

#define RENDERER_DEPTH_TEST 0
#define RENDERER_TRIANGLE 1

#define RENDERER RENDERER_TRIANGLE

#if RENDERER == RENDERER_DEPTH_TEST
#include "../Renderer/DepthTestRenderer.hpp"

#define RENDERER_CLASS DepthTestRenderer
#endif

#if RENDERER == RENDERER_TRIANGLE
	#include "../Renderer/TriangleRenderer.hpp"

	#define RENDERER_CLASS TriangleRenderer
#endif

namespace ProjectA {
	class SampleLayer : public Layer {
		public:
			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) {
				EventDispatcher eventDispatcher(event);

				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
			}

			bool onWindowResize(WindowResizeEvent& event) {
				renderer.resize(event);

				return true;
			}

		private:
			RENDERER_CLASS renderer;
	};
}