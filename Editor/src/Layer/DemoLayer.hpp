#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoLayer : public Layer {
		private:
			TriangleDemoRenderer renderer;

		public:
			DemoLayer();

			virtual ~DemoLayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			void onEvent(Event& event) override;
	};
}