#pragma once

#include "Engine/ECS/Pool/ComponentPool.hpp"

namespace Trident {
	class ECS {
		public:
			std::vector<Entity> availableEntities;
			Entity nextEntity = 0;
			std::vector<scope<Pool>> componentPools;

		public:
			Entity createEntity();

			// Allocates space for a component pool of a new type.
			template<typename ComponentType>
			void registerComponent() {
				ENGINE_ASSERT(componentPools.size() < MAX_COMPONENT_POOLS, "Component pool limit reached.");

				const uint32 componentPoolIndex = getComponentPoolIndex<ComponentType>();

				if (componentPoolIndex < componentPools.size()) {
					ENGINE_WARNING("Component type is already registered.");

					return;
				}

				// Allocate space for new pool.
				componentPools.resize(componentPools.size() + 1);
				componentPools[componentPoolIndex] = createScope<ComponentPool<ComponentType>>();
			}

			template<typename ComponentType>
			const ComponentType& getComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<ComponentType>& componentPool = getComponentPool<ComponentType>();

				return componentPool.getComponent(entity);
			}

			template<typename ComponentType>
			ComponentType& getComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<ComponentType>& componentPool = getComponentPool<ComponentType>();

				return componentPool.getComponent(entity);
			}

			template<typename ComponentType>
			void setComponent(Entity entity, const ComponentType& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<ComponentType>& componentPool = getComponentPool<ComponentType>();
				componentPool.setComponent(entity, component);
			}

			template<typename ComponentType>
			ComponentPool<ComponentType>& getComponentPool() const {
				const Entity componentPoolIndex = getComponentPoolIndex<ComponentType>();

				ENGINE_ASSERT(componentPoolIndex < componentPools.size(), "Component type does not exist and can't be retrieved.");

				return *static_cast<ComponentPool<ComponentType>*>(componentPools[componentPoolIndex].get());
			}

			template<typename ComponentType>
			void resetComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<ComponentType>& componentPool = getComponentPool<ComponentType>();
				componentPool.resetComponent(entity);
			}

		private:
			static uint32 getNextComponentPoolIndex();

			// Returns a unique index for each distinct component type.
			template<typename ComponentType>
			static uint32 getComponentPoolIndex() {
				static uint32 componentPoolIndex = getNextComponentPoolIndex();

				return componentPoolIndex;
			}

		private:
			static constexpr uint8 MAX_COMPONENT_POOLS = 64;
	};
}