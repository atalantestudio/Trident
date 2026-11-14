// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void broadPhase(PhysicsSimulation2D& simulation) {
		CollisionPair2D collisionPair;
		uint32 indexA = 0;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < simulation.broadPhase.usedDynamicShapeIdCount) {
			collisionPair.shapeIdA = simulation.broadPhase.dynamicShapeIds[indexA];

			uint32 indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < simulation.broadPhase.usedStaticShapeIdCount) {
				collisionPair.shapeIdB = simulation.broadPhase.staticShapeIds[indexB];

				simulation.collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				collisionPair.shapeIdB = simulation.broadPhase.dynamicShapeIds[indexB];

				simulation.collisionPairs.emplace_back(collisionPair);

				indexB += 1;
			}

			indexA += 1;
		}
	}
}