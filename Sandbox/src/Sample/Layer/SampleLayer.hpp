#pragma once

#include "Engine/Layer/Layer.hpp"

#include "../Renderer/DepthTestRenderer.hpp"

namespace ProjectA {
	class SampleLayer : public Layer {
		public:
			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const final override {
				renderer.render(commandEncoder);
			}

		private:
			DepthTestRenderer renderer;
	};
}