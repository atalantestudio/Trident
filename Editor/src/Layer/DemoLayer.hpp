#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TextureDemoRenderer renderer;

		public:
			DemoLayer();

			virtual ~DemoLayer() = default;

			void bind() override;
			void unbind() override;

			void update(const float32 deltaTime) override;

			void renderImGui() override;

			void onEvent(Event& event) override;
	};
}