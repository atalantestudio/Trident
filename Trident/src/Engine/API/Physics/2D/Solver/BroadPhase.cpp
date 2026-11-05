// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	// TODO(physics/2d/broad-phase): Improve broad-phase.
	void broadPhase(PhysicsSimulation2D& simulation) {
		CollisionPair2D pair;
		uint32 shapeIdIndexA = 0;
		uint32 shapeIdIndexB;

		while (shapeIdIndexA < simulation.broadPhase.usedStaticShapeIdCount) {
			pair.shapeIdA = simulation.broadPhase.staticShapeIds[shapeIdIndexA];

			const Shape2D& shape = simulation.shapes[pair.shapeIdA];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndexA += 1;

				continue;
			}

			shapeIdIndexB = 0;

			while (shapeIdIndexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.dynamicShapeIds[shapeIdIndexB];

				const Shape2D& shape = simulation.shapes[pair.shapeIdB];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					shapeIdIndexB += 1;

					continue;
				}

				simulation.collisionPairs.emplace_back(pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexA += 1;
		}

		shapeIdIndexA = 0;

		while (shapeIdIndexA < simulation.broadPhase.usedKinematicShapeIdCount) {
			pair.shapeIdA = simulation.broadPhase.kinematicShapeIds[shapeIdIndexA];

			const Shape2D& shape = simulation.shapes[pair.shapeIdA];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndexA += 1;

				continue;
			}

			while (shapeIdIndexB < simulation.broadPhase.usedDynamicShapeIdCount) {
				pair.shapeIdB = simulation.broadPhase.dynamicShapeIds[shapeIdIndexB];

				const Shape2D& shape = simulation.shapes[pair.shapeIdB];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					shapeIdIndexB += 1;

					continue;
				}

				simulation.collisionPairs.emplace_back(pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexA += 1;
		}

		shapeIdIndexA = 0;

		while (shapeIdIndexA < simulation.broadPhase.usedDynamicShapeIdCount) {
			pair.shapeIdA = simulation.broadPhase.dynamicShapeIds[shapeIdIndexA];

			const Shape2D& shape = simulation.shapes[pair.shapeIdA];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndexA += 1;

				continue;
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

				simulation.collisionPairs.emplace_back(pair);

				shapeIdIndexB += 1;
			}

			shapeIdIndexA += 1;
		}
	}
}