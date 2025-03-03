#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class TriangleDemoLayer : public Layer {
		private:
			TriangleDemoRenderer renderer;

		public:
			TriangleDemoLayer();

			virtual ~TriangleDemoLayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			virtual void renderImGui() override;

			void onEvent(Event& event) override;
	};
}