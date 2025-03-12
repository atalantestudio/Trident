#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class GUILayer : public Layer {
		private:
			ECS ecs;
			GUIRenderer renderer;

		public:
			GUILayer();

			virtual ~GUILayer() = default;

			void bind() override;
			void unbind() override;

			void update(const float32 deltaTime) override;

			void renderImGui() override;

			void onEvent(Event& event) override;
	};
}