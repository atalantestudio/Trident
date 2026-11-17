// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void addPair(PhysicsSimulation2D& simulation, CollisionPair2D pair) {
		const Shape2D& shapeA = simulation.shapes[pair.shapeIdA];
		const Shape2D& shapeB = simulation.shapes[pair.shapeIdB];

		// TODO(physics/2d): Fix broadphase order.
		// std::swap(pair.shapeIdA, pair.shapeIdB);

		if (shapeA.type > shapeB.type) {
			std::swap(pair.shapeIdA, pair.shapeIdB);
		}

		simulation.collisionPairs.emplace_back(pair);
	}

	void broadPhase(PhysicsSimulation2D& simulation) {
		CollisionPair2D pair;
		uint32 indexA = 0;

		// TODO(physics/2d/broad-phase): Only register potentially overlapping entities.
		while (indexA < simulation.broadPhase.usedDynamicShapeIdCount) {
			pair.shapeIdA = simulation.broadPhase.dynamicShapeIds[indexA];

			uint32 indexB = 0;

			// Register all dynamic-static entity pairs.
			while (indexB < simulation.broadPhase.usedStaticShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.staticShapeIds[indexB];

				addPair(simulation, pair);

				indexB += 1;
			}

			indexB = indexA + 1;

			// Register all dynamic-dynamic entity pairs.
			while (indexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.dynamicShapeIds[indexB];

				addPair(simulation, pair);

				indexB += 1;
			}

			indexA += 1;
		}
	}
}