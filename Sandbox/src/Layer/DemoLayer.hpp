#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TriangleDemoRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TriangleDemoRenderer renderer;

		public:
			DemoLayer(/*const TrueTypeFont& font*/);

			virtual ~DemoLayer() = default;

			void update(const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}