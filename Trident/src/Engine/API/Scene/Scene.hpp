// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/ECS.hpp"

namespace Trident {
	class Scene {
		public:
			template<typename ComponentType>
			const ComponentPool<ComponentType>& getComponentPool() const {
				return ecs.getComponentPool<ComponentType>();
			}

			template<typename ComponentType>
			const ComponentType& getComponent(Entity entity) const {
				return ecs.getComponent<ComponentType>(entity);
			}

			template<typename ComponentType>
			ComponentType& getComponent(Entity entity) {
				return ecs.getComponent<ComponentType>(entity);
			}

			Entity getLastEntity() const {
				return ecs.nextEntity - 1;
			}

		protected:
			ECS ecs;
	};
}