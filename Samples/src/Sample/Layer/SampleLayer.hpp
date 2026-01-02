#pragma once

#include "Engine/Layer/Layer.hpp"

#include "../Sample/Empty.hpp"

namespace ProjectA {
	class SampleLayer : public Layer {
		public:
			void build() override {
				renderer.setLayer(this);
				renderer.build();
			}

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const override {
				renderer.render(commandEncoder);
			}

			void onEvent(Event& event) override {
				EventDispatcher eventDispatcher(event);

				eventDispatcher.dispatch<WindowResizeEvent>(BIND(onWindowResize));
			}

		private:
			bool onWindowResize(WindowResizeEvent& event) {
				renderer.resize(event);

				return true;
			}

		private:
			SampleRenderer renderer;
	};
}