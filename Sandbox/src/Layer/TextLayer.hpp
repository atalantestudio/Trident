#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TextRenderer.hpp"

namespace ProjectA {
	class TextLayer : public Layer {
		public:
			TextLayer(const TrueTypeFont& font);

			virtual ~TextLayer() = default;

			void render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) override;

		private:
			TextRenderer renderer;
	};
}