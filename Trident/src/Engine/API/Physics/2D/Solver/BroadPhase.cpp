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
		uint32 shapeIdIndexA = 0;

		while (shapeIdIndexA < simulation.broadPhase.usedDynamicShapeIdCount) {
			CollisionPair2D pair;
			pair.shapeIdA = simulation.broadPhase.dynamicShapeIds[shapeIdIndexA];

			const Shape2D& shape = simulation.shapes[pair.shapeIdA];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndexA += 1;

				continue;
			}

			uint32 shapeIdIndexB = 0;

			while (shapeIdIndexB < simulation.broadPhase.usedStaticShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.staticShapeIds[shapeIdIndexB];

				const Shape2D& shape = simulation.shapes[pair.shapeIdB];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					shapeIdIndexB += 1;

					continue;
				}

				addPair(simulation, pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexB = 0;

			while (shapeIdIndexB < simulation.broadPhase.usedKinematicShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.kinematicShapeIds[shapeIdIndexB];

				const Shape2D& shape = simulation.shapes[pair.shapeIdB];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					shapeIdIndexB += 1;

					continue;
				}

				addPair(simulation, pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexB = shapeIdIndexA + 1;

			while (shapeIdIndexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.dynamicShapeIds[shapeIdIndexB];

				const Shape2D& shape = simulation.shapes[pair.shapeIdB];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					shapeIdIndexB += 1;

					continue;
				}

				addPair(simulation, pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexA += 1;
		}
	}
}