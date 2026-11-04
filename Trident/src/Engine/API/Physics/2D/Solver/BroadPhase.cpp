// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	void broadPhase(PhysicsSimulation2D& simulation) {
		const TagPool<DynamicEntityTag>& dynamicPool = simulation.scene->ecs.getTagPool<DynamicEntityTag>();
		const TagPool<StaticEntityTag>& staticPool = simulation.scene->ecs.getTagPool<StaticEntityTag>();

		EntityPair entityPair;
		uint32 indexA = 0;
		uint32 indexB;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < dynamicPool.tagCount) {
			entityPair.A = dynamicPool.tagIndexToEntity[indexA];

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < dynamicPool.tagCount) {
				entityPair.B = dynamicPool.tagIndexToEntity[indexB];

				simulation.entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < staticPool.tagCount) {
				entityPair.B = staticPool.tagIndexToEntity[indexB];

				simulation.entityPairs.emplace_back(entityPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}
}