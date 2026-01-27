#include "pch.hpp"

#include "Engine/ECS/ECS.hpp"

namespace Trident {
	Entity ECS::createEntity() {
		if (availableEntities.empty()) {
			// Take the next entity from the iterator.
			ENGINE_ASSERT(nextEntity < MAX_ENTITIES, "Entity limit reached.");

			const Entity entity = nextEntity;

			nextEntity += 1;

			return entity;
		}

		// Take the next entity from the available list.
		const Entity entity = availableEntities.back();

		availableEntities.pop_back();

		return entity;
	}

	uint32 ECS::getNextComponentPoolIndex() {
		static uint32 componentPoolIndex = 0;

		const uint32 index = componentPoolIndex;

		componentPoolIndex += 1;

		return index;
	}
}