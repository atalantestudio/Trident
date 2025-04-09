#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TextureDemoRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TextureDemoRenderer renderer;

		public:
			DemoLayer(const TrueTypeFont& font);

			virtual ~DemoLayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}