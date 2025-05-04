#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TriangleDemoRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		public:
			DemoLayer(/*const TrueTypeFont& font*/);

			virtual ~DemoLayer() = default;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) override;

		private:
			TriangleDemoRenderer renderer;
	};
}