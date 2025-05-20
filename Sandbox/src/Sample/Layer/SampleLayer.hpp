#pragma once

#include "Engine/Layer/Layer.hpp"

#include "../Renderer/TriangleRenderer.hpp"

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

			bool onWindowResize(WindowResizeEvent& event) const {
				renderer.resize(event);

				return true;
			}

		private:
			TriangleRenderer renderer;
	};
}