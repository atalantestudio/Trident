#pragma once

#include "ProjectA.hpp"

#include "../Renderer/TriangleDemoRenderer.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TriangleDemoRenderer renderer;

		public:
			DemoLayer();

			virtual ~DemoLayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
	};
}