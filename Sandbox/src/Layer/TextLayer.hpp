#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TextRenderer.hpp"

namespace ProjectA {
	class TextLayer : public Layer {
		private:
			TextRenderer renderer;

		public:
			TextLayer(const TrueTypeFont& font);

			virtual ~TextLayer() = default;

			void update(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}