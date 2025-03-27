#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/ECS/Pool/Pool.hpp"

namespace ProjectA {
	template<typename T>
	class ComponentPool : public Pool {
		public:
			std::array<Entity, MAX_ENTITIES> entityToComponentIndex; // Sparse array
			chain<uint32> componentIndexToEntity; // Dense array
			chain<T> components; // Dense array

			// Number of currently alive components.
			uint32 componentCount = 0;

		public:
			ComponentPool() {
				// Fill array with null index.
				for (Entity& entity : entityToComponentIndex) {
					entity = NULL_COMPONENT_INDEX;
				}
			}

			// Returns the component of type T belonging to the provided entity.
			const T& getComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != NULL_COMPONENT_INDEX, "Entity [] has no components.", entity);

				return entityComponents[componentIndex].component;
			}

			// Returns the component of type T belonging to the provided entity.
			T& getComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != NULL_COMPONENT_INDEX, "Entity [] has no components.", entity);

				return entityComponents[componentIndex].component;
			}

			// TODO: hasComponent.

			// Sets or replaces the component of type T belonging to the provided entity.
			void setComponent(Entity entity, const T& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				if (componentIndex != NULL_COMPONENT_INDEX) {
					// Replace (overwrite) component.
					components[componentIndex] = component;

					return;
				}

				// Set (add) component.
				entityToComponentIndex[entity] = componentCount;

				components.emplace_back(component);
				componentIndexToEntity.emplace_back(entity);

				componentCount += 1;
			}

			// Erases the component of type T belonging to the provided entity.
			void resetComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "The entity [] is overflowing.", entity);

				const Entity componentIndex = entityToComponentIndex[entity];

				if (componentIndex == NULL_COMPONENT_INDEX) {
					ENGINE_ERROR("The component for the entity [] is already reset.", entity);

					return;
				}

				// Don't touch the deleted data; instead wait for another component to overwrite it.
				componentCount -= 1;

				T& component = components[componentIndex];
				T& lastComponent = components[componentCount];

				// Move the component to be deleted to the end of the list.
				std::swap(component, lastComponent);

				const Entity lastComponentEntity = componentIndexToEntity[componentCount];

				// Since the components have been swapped, the entity pointing to lastComponent
				// now points to the component at componentIndex.
				entityToComponentIndex[lastComponentEntity] = componentIndex;

				// Reset the index of the component to be deleted.
				entityToComponentIndex[entity] = NULL_COMPONENT_INDEX;
			}

		private:
			static constexpr uint32 NULL_COMPONENT_INDEX = std::numeric_limits<Entity>::max();
	};
}