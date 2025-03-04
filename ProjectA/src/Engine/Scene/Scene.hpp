#pragma once

#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	class Scene {
		public:
			Scene();

			~Scene();

			void onUpdate(const float32 deltaTime);

		private:
			ECS ecs;
	};
}