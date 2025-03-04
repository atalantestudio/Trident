#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class ECSDemoLayer : public Layer {
		private:
			ECS ecs;
			const Entity triangleEntity;

		public:
			ECSDemoLayer();

			virtual ~ECSDemoLayer() = default;

			void bind() override;
			void unbind() override;

			void update(const float32 deltaTime) override;

			virtual void renderImGui() override;

			void onEvent(Event& event) override;
	};
}