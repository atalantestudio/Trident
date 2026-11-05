// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	// TODO(physics/2d/broad-phase): Improve broad-phase.
	void broadPhase(PhysicsSimulation2D& simulation) {
		CollisionPair2D pair;
		pair.shapeIdA = 0;

		while (pair.shapeIdA < simulation.broadPhase.staticShapeIndices.count()) {
			const Shape2D& shape = simulation.shapes[simulation.broadPhase.staticShapeIndices[pair.shapeIdA]];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				pair.shapeIdA += 1;

				continue;
			}

			pair.shapeIdB = 0;

			while (pair.shapeIdB < simulation.broadPhase.dynamicShapeIndices.count()) {
				const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIndices[pair.shapeIdB]];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					pair.shapeIdB += 1;

					continue;
				}

				simulation.collisionPairs.emplace_back(pair);

				pair.shapeIdB += 1;
			}

			pair.shapeIdA += 1;
		}

		pair.shapeIdA = 0;

		while (pair.shapeIdA < simulation.broadPhase.kinematicShapeIndices.count()) {
			const Shape2D& shape = simulation.shapes[simulation.broadPhase.kinematicShapeIndices[pair.shapeIdA]];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				pair.shapeIdA += 1;

				continue;
			}

			while (pair.shapeIdB < simulation.broadPhase.dynamicShapeIndices.count()) {
				const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIndices[pair.shapeIdB]];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					pair.shapeIdB += 1;

					continue;
				}

				simulation.collisionPairs.emplace_back(pair);

				pair.shapeIdB += 1;
			}

			pair.shapeIdA += 1;
		}

		pair.shapeIdA = 0;

		while (pair.shapeIdA < simulation.broadPhase.dynamicShapeIndices.count()) {
			const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIndices[pair.shapeIdA]];
			const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				pair.shapeIdA += 1;

				continue;
			}

			pair.shapeIdB = pair.shapeIdA + 1;

			while (pair.shapeIdB < simulation.broadPhase.dynamicShapeIndices.count()) {
				const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIndices[pair.shapeIdB]];
				const RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

				if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
					pair.shapeIdB += 1;

					continue;
				}

				simulation.collisionPairs.emplace_back(pair);

				pair.shapeIdB += 1;
			}

			pair.shapeIdA += 1;
		}
	}
}