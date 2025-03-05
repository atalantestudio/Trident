#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		private:
			GUIRenderer renderer;

		public:
			GUILayer();

			virtual ~GUILayer() = default;

			void bind() override;
			void unbind() override;

			void update(const float32 deltaTime) override;

			virtual void renderImGui() override;

			void onEvent(Event& event) override;
	};
}