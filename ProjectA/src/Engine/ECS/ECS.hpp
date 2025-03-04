#pragma once

#include "pch.hpp"

namespace ProjectA {
	typedef uint32 Entity;
	typedef uint32 ComponentType;

	static constexpr ComponentType MAX_COMPONENT_POOLS = 64;
	static constexpr ComponentType MAX_COMPONENT_PER_ENTITY = 64;

	static constexpr Entity MAX_ENTITIES = 100000;

	template<typename T>
	struct EntityComponent {
		Entity entity;
		T component;
	};

	class Pool {};

	template<typename T>
	class ComponentPool : public Pool {
		public:
			static constexpr Entity overflowComponentIndex = std::numeric_limits<Entity>::max();

			std::vector<Entity> entityToComponentIndex; // Sparse array
			std::vector<EntityComponent<T>> entityComponents; // Dense array
			Entity aliveComponents = 0;

		public:
			ComponentPool() {
				ENGINE_TRACE("[ComponentPool]");

				entityToComponentIndex.resize(MAX_ENTITIES);

				// Fill entity -> component index with invalid ID.
				for (Entity& i : entityToComponentIndex) {
					i = overflowComponentIndex;
				}
			}

			// Returns the component of type T belonging to the provided entity.
			// If the entity doesn't have a component of this type, null will be returned.
			T* getComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Overflowing entity.");

				const Entity componentIndex = entityToComponentIndex[entity];

				if (componentIndex == overflowComponentIndex) {
					ENGINE_ERROR("Entity [] has no components.", entity);

					return nullptr;
				}

				return &(entityComponents[componentIndex].component);
			}

			void setComponent(const Entity entity, const T& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Overflowing entity.");

				Entity componentIndex = entityToComponentIndex[entity];

				if (componentIndex == overflowComponentIndex) {
					// Add component.
					componentIndex = aliveComponents;

					entityToComponentIndex[entity] = componentIndex;

					EntityComponent<T> entityComponent{};
					entityComponent.entity = entity;
					entityComponent.component = component;

					entityComponents.emplace_back(entityComponent);

					// Account for one more alive entity component.
					aliveComponents++;
				}
				else {
					// Overwrite existing component.
					entityComponents[componentIndex].component = component;
				}
			}

			void resetComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Overflowing entity.");

				const Entity componentIndex = entityToComponentIndex[entity];

				if (componentIndex == overflowComponentIndex) {
					ENGINE_WARN("Entity is already reset (no component found).");

					return;
				}

				const EntityComponent& entityComponent = entityComponents[componentIndex];
				const EntityComponent& lastEntityComponent = entityComponents.back();

				// Put the component to be deleted at the end of the list.
				std::swap(entityComponent, lastEntityComponent);

				// Fix mapping of the swapped back component.
				entityToComponentIndex[lastEntityComponent.entity] = componentIndex;

				// Reset the index of the component to be deleted.
				entityToComponentIndex[entity] = overflowComponentIndex;

				// Account for one less entity component.
				// Don't touch the data; instead wait for another component to overwrite it.
				aliveComponents--;
			}
	};

	class ECS {
		public:
			std::vector<Entity> availableEntities;
			Entity nextEntity = 0;
			std::vector<std::unique_ptr<Pool>> componentPools;

		public:
			Entity createEntity() {
				Entity entity = 0;

				if (!availableEntities.empty()) {
					// Take the next entity from the available list.
					entity = availableEntities.back();

					availableEntities.pop_back();
				}
				else {
					// Take the next entity from the iterator.
					ENGINE_ASSERT(nextEntity < MAX_ENTITIES, "Entity limit reached.");

					entity = nextEntity;

					nextEntity++;
				}

				return entity;
			}

			// Allocates space for a pool of component type T.
			template<typename T>
			void defineComponent() {
				ENGINE_ASSERT(componentPools.size() < MAX_COMPONENT_POOLS, "Component pool limit reached.");

				const uint32 componentPoolIndex = getComponentPoolIndex<T>();

				if (componentPoolIndex < componentPools.size()) {
					ENGINE_WARN("Component type is already registered.");

					return;
				}

				ENGINE_TRACE("Allocating space for new component pool (index = []).", componentPoolIndex);

				// Allocate space for new pool.
				componentPools.resize(componentPools.size() + 1);
				componentPools[componentPoolIndex] = std::make_unique<ComponentPool<T>>();
			}

			template<typename T>
			T& getComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				return *componentPool.getComponent(entity);
			}

			template<typename T>
			void setComponent(const Entity entity, const T& component) {
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

				return *static_cast<ComponentPool<T>*>(componentPools[componentPoolIndex].get());
			}

		private:
			static uint32 getNextComponentPoolIndex() {
				static uint32 componentPoolIndex = 0;

				return componentPoolIndex++;
			}

			// Returns a unique index for each distinct type T.
			template<typename T>
			static uint32 getComponentPoolIndex() {
				static uint32 componentPoolIndex = getNextComponentPoolIndex();

				return componentPoolIndex;
			}
	};
}