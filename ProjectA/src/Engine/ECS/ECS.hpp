#pragma once

#include "Engine/ECS/Pool/ComponentPool.hpp"

namespace ProjectA {
	class ECS {
		public:
			std::vector<Entity> availableEntities;
			Entity nextEntity = 0;
			std::vector<scope<Pool>> componentPools;

		public:
			Entity createEntity();

			// Allocates space for a pool of component type T.
			template<typename T>
			void registerComponent() {
				ENGINE_ASSERT(componentPools.size() < MAX_COMPONENT_POOLS, "Component pool limit reached.");

				const uint32 componentPoolIndex = getComponentPoolIndex<T>();

				if (componentPoolIndex < componentPools.size()) {
					ENGINE_WARN("Component type is already registered.");

					return;
				}

				// Allocate space for new pool.
				componentPools.resize(componentPools.size() + 1);
				componentPools[componentPoolIndex] = createScope<ComponentPool<T>>();
			}

			// TODO: Const variant.
			template<typename T>
			T& getComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				return componentPool.getComponent(entity);
			}

			template<typename T>
			void setComponent(Entity entity, const T& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				componentPool.setComponent(entity, component);
			}

			template<typename T>
			ComponentPool<T>& getComponentPool() const {
				const Entity componentPoolIndex = getComponentPoolIndex<T>();

				if (componentPoolIndex >= componentPools.size() || !componentPools[componentPoolIndex]) {
					ENGINE_ASSERT(false, "Component type does not exist and can't be retrieved.");
				}

				Pool* componentPool = componentPools[componentPoolIndex].get();

				return *static_cast<ComponentPool<T>*>(componentPool);
			}

			template<typename T>
			void resetComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				componentPool.resetComponent(entity);
			}

		private:
			static uint32 getNextComponentPoolIndex();

			// Returns a unique index for each distinct type T.
			template<typename T>
			static uint32 getComponentPoolIndex() {
				static uint32 componentPoolIndex = getNextComponentPoolIndex();

				return componentPoolIndex;
			}

		private:
			static constexpr uint8 MAX_COMPONENT_POOLS = 64;
	};
}