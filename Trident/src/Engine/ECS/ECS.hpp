// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Pool/ComponentPool.hpp"
#include "Engine/ECS/Pool/TagPool.hpp"

namespace Trident {
	class ECS {
		private:
			static constexpr uint8 MAX_COMPONENT_POOLS = 64;

		private:
			static uint32 getNextComponentPoolIndex();

			// Returns a unique index for each distinct component type.
			template<typename ComponentType>
			static uint32 getComponentPoolIndex() {
				static uint32 componentPoolIndex = getNextComponentPoolIndex();

				return componentPoolIndex;
			}

		public:
			/// Creates and returns a new entity.
			Entity createEntity();

			/// Allocates space for a new component pool.
			template<typename ComponentType>
			void registerComponentPool() {
				ENGINE_ASSERT(componentPools.size() < MAX_COMPONENT_POOLS, "Component pool limit reached.");

				const uint32 componentPoolIndex = getComponentPoolIndex<ComponentType>();

				if (componentPoolIndex < componentPools.size()) {
					if (componentPools[componentPoolIndex] != nullptr) {
						ENGINE_WARNING("Component type is already registered.");

						return;
					}
				}
				else {
					// NOTE: Depending on the number returned by ECS::getComponentPoolIndex, multiple pools can be allocated at once.
					componentPools.resize(componentPoolIndex + 1);
				}

				componentPools[componentPoolIndex] = createScope<ComponentPool<ComponentType>>();
			}

			/// Allocates space for a new tag pool.
			template<typename TagType>
			void registerTagPool() {
				ENGINE_ASSERT(componentPools.size() < MAX_COMPONENT_POOLS, "Component pool limit reached.");

				const uint32 componentPoolIndex = getComponentPoolIndex<TagType>();

				if (componentPoolIndex < componentPools.size()) {
					if (componentPools[componentPoolIndex] != nullptr) {
						ENGINE_WARNING("Tag type is already registered.");

						return;
					}
				}
				else {
					// NOTE: Depending on the number returned by ECS::getComponentPoolIndex, multiple pools can be allocated at once.
					componentPools.resize(componentPoolIndex + 1);
				}

				componentPools[componentPoolIndex] = createScope<TagPool<TagType>>();
			}

			template<typename Component>
			bool hasComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				return getComponentPool<Component>().hasComponent(entity);
			}

			template<typename TagType>
			bool hasTag(Entity entity) const {
				ENGINE_STATIC_ASSERT(sizeof(TagType) == 1, "Invalid tag.");
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				return getTagPool<TagType>().hasTag(entity);
			}

			template<typename ComponentType>
			const ComponentType& getComponent(Entity entity) const {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				return getComponentPool<ComponentType>().getComponent(entity);
			}

			template<typename ComponentType>
			ComponentType& getComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				return getComponentPool<ComponentType>().getComponent(entity);
			}

			template<typename ComponentType>
			void setComponent(Entity entity, const ComponentType& component) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				getComponentPool<ComponentType>().setComponent(entity, component);
			}

			template<typename TagType>
			void setTag(Entity entity) {
				ENGINE_STATIC_ASSERT(sizeof(TagType) == 1, "Invalid tag component.");
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				getTagPool<TagType>().setTag(entity);
			}

			template<typename ComponentType>
			ComponentPool<ComponentType>& getComponentPool() const {
				const Entity componentPoolIndex = getComponentPoolIndex<ComponentType>();

				ENGINE_ASSERT(componentPoolIndex < componentPools.size(), "Component type does not exist and can't be retrieved.");

				return *static_cast<ComponentPool<ComponentType>*>(componentPools[componentPoolIndex].get());
			}

			template<typename TagType>
			TagPool<TagType>& getTagPool() const {
				const Entity tagPoolIndex = getComponentPoolIndex<TagType>();

				ENGINE_ASSERT(tagPoolIndex < componentPools.size(), "Tag type does not exist and can't be retrieved.");

				return *static_cast<TagPool<TagType>*>(componentPools[tagPoolIndex].get());
			}

			/// Erases the component belonging to the provided entity.
			template<typename ComponentType>
			void resetComponent(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				getComponentPool<ComponentType>().resetComponent(entity);
			}

			/// Erases the tag belonging to the provided entity.
			template<typename TagType>
			void resetTag(Entity entity) {
				ENGINE_ASSERT(entity < MAX_ENTITIES, "Entity [] doesn't exist.", entity);

				getTagPool<TagType>().resetTag(entity);
			}

		public:
			std::vector<Entity> availableEntities;
			Entity nextEntity = 0;
			std::vector<scope<Pool>> componentPools;
	};
}