#pragma once

#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	class Scene {
		public:
			template<typename ComponentType>
			const ComponentPool<ComponentType>& getComponentPool() const {
				return ecs.getComponentPool<ComponentType>();
			}

			Entity getLastEntity() const {
				return ecs.nextEntity - 1;
			}

		protected:
			ECS ecs;
	};
}