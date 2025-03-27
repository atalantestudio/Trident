#include "pch.hpp"

#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	Entity ECS::createEntity() {
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

			nextEntity += 1;
		}

		return entity;
	}

	uint32 ECS::getNextComponentPoolIndex() {
		static uint32 componentPoolIndex = 0;

		const uint32 index = componentPoolIndex;

		componentPoolIndex += 1;

		return index;
	}
}