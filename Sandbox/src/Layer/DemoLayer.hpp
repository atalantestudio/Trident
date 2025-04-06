#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TextRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TextRenderer renderer;

		public:
			DemoLayer(const TrueTypeFont& font);

			virtual ~DemoLayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}