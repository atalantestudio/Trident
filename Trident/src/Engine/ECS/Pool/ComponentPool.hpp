// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Entity.hpp"
#include "Engine/ECS/Pool/Pool.hpp"

namespace Trident {
	template<typename ComponentType>
	class ComponentPool : public Pool {
		private:
			static constexpr uint32 NULL_COMPONENT_INDEX = max<uint32>();

		public:
			explicit ComponentPool() {
				std::memset(&entityToComponentIndex, NULL_COMPONENT_INDEX, MAX_ENTITIES * sizeof(uint32));
			}

			bool hasComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				return entityToComponentIndex[entity] != NULL_COMPONENT_INDEX;
			}

			/// Returns the component belonging to the provided entity.
			const ComponentType& getComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != NULL_COMPONENT_INDEX, "Entity [] has no components.", entity);

				return components[componentIndex];
			}

			/// Returns the component belonging to the provided entity.
			ComponentType& getComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				ENGINE_ASSERT(componentIndex != NULL_COMPONENT_INDEX, "Entity [] has no components.", entity);

				return components[componentIndex];
			}

			/// Sets or replaces the component belonging to the provided entity.
			void setComponent(Entity entity, const ComponentType& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				if (componentIndex != NULL_COMPONENT_INDEX) {
					// Replace (overwrite) component.
					components[componentIndex] = component;

					return;
				}

				entityToComponentIndex[entity] = componentCount;

				components.emplace_back(component);
				componentIndexToEntity.emplace_back(entity);

				componentCount += 1;
			}

			/// Erases the component belonging to the provided entity.
			void resetComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] is overflowing.", entity);

				const uint32 componentIndex = entityToComponentIndex[entity];

				if (componentIndex == NULL_COMPONENT_INDEX) {
					ENGINE_ERROR("The component for entity [] is already reset.", entity);

					return;
				}

				// Don't touch the deleted data; instead wait for another component to overwrite it.
				componentCount -= 1;

				ComponentType& component = components[componentIndex];
				ComponentType& lastComponent = components[componentCount];

				// Move the component to be deleted to the end of the list.
				std::swap(component, lastComponent);

				const Entity lastComponentEntity = componentIndexToEntity[componentCount];

				// Since the components have been swapped, the entity pointing to lastComponent
				// now points to the component at componentIndex.
				entityToComponentIndex[lastComponentEntity] = componentIndex;

				// Reset the index of the component to be deleted.
				entityToComponentIndex[entity] = NULL_COMPONENT_INDEX;
			}

		public:
			// (sparse array)
			std::array<uint32, MAX_ENTITIES> entityToComponentIndex;

			// (dense array)
			std::vector<Entity> componentIndexToEntity;

			// (dense array)
			std::vector<ComponentType> components;

			// Number of currently alive components.
			uint32 componentCount = 0;
	};
}