#pragma once

#include "pch.hpp"

namespace ProjectA {
	typedef uint32 Entity;
	typedef uint32 ComponentType;

	static constexpr Entity MAX_ENTITIES = 100000;

	static constexpr ComponentType MAX_COMPONENT_POOLS = 64;
	static constexpr ComponentType MAX_COMPONENT_PER_ENTITY = 64;

	class Pool {};

	template<typename T>
	class ComponentPool : public Pool {
		public:
			std::array<Entity, MAX_ENTITIES> entityToComponentIndex; // Sparse array
			std::vector<uint32> componentIndexToEntity; // Dense array
			std::vector<T> components; // Dense array

			// Number of currently alive components.
			uint32 componentCount = 0;

		public:
			ComponentPool() {
				ENGINE_TRACE("[ComponentPool]");

				// Fill array with null index.
				for (Entity& entity : entityToComponentIndex) {
					entity = nullComponentIndex;
				}
			}

			// Returns the component of type T belonging to the provided entity.
			const T& getComponent(const Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != nullComponentIndex, "Entity [] has no components.", entity);

				return entityComponents[componentIndex].component;
			}

			// Returns the component of type T belonging to the provided entity.
			T& getComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != nullComponentIndex, "Entity [] has no components.", entity);

				return entityComponents[componentIndex].component;
			}

			// TODO: hasComponent.

			// Sets or replaces the component of type T belonging to the provided entity.
			void setComponent(const Entity entity, const T& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				if (componentIndex != nullComponentIndex) {
					// Replace (overwrite) component.
					components[componentIndex] = component;

					return;
				}

				// Set (add) component.
				entityToComponentIndex[entity] = componentCount;

				components.emplace_back(component);
				componentIndexToEntity.emplace_back(entity);

				componentCount++;
			}

			// Erases the component of type T belonging to the provided entity.
			void resetComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				if (componentIndex == nullComponentIndex) {
					ENGINE_ERROR("The component for the entity [] is already reset.", entity);

					return;
				}

				// Don't touch the deleted data; instead wait for another component to overwrite it.
				componentCount--;

				T& component = components[componentIndex];
				T& lastComponent = components[componentCount];

				// Move the component to be deleted to the end of the list.
				std::swap(component, lastComponent);

				const Entity lastComponentEntity = componentIndexToEntity[componentCount];

				// Since the components have been swapped, the entity pointing to lastComponent
				// now points to the component at componentIndex.
				entityToComponentIndex[lastComponentEntity] = componentIndex;

				// Reset the index of the component to be deleted.
				entityToComponentIndex[entity] = nullComponentIndex;
			}

		private:
			static constexpr uint32 nullComponentIndex = std::numeric_limits<Entity>::max();
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

				// Allocate space for new pool.
				componentPools.resize(componentPools.size() + 1);
				componentPools[componentPoolIndex] = std::make_unique<ComponentPool<T>>();
			}

			// TODO: Const variant.
			template<typename T>
			T& getComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				return componentPool.getComponent(entity);
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

			template<typename T>
			void resetComponent(const Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Invalid entity.");

				ComponentPool<T>& componentPool = getComponentPool<T>();

				componentPool.resetComponent(entity);
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