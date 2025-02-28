#pragma once

#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	class Scene {
		public:
			Scene();

			~Scene();
		private:
			ECS ecs;
	};
}