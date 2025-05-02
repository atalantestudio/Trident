#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TextureArrayRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		public:
			DemoLayer(/*const TrueTypeFont& font*/);

			virtual ~DemoLayer() = default;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;

		private:
			TextureArrayRenderer renderer;
	};
}