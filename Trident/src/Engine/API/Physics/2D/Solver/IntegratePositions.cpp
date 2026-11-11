// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void integratePositionsType(PhysicsSimulation2D& simulation, view<ShapeID2D> shapeIds) {
		uint32 shapeIdIndex = 0;

		while (shapeIdIndex < shapeIds.count()) {
			const Shape2D& shape = simulation.shapes[shapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndex += 1;

				continue;
			}

			if (rigidBody.flags & RigidBodyFlags2D::LOCK_LINEAR_X_MOVEMENT) {
				rigidBody.linearVelocity.x = 0.0f;
			}

			if (rigidBody.flags & RigidBodyFlags2D::LOCK_LINEAR_Y_MOVEMENT) {
				rigidBody.linearVelocity.x = 0.0f;
			}

			if (rigidBody.flags & RigidBodyFlags2D::LOCK_ANGULAR_MOVEMENT) {
				rigidBody.angularVelocity = 0.0f;
			}

			rigidBody.transform.position += rigidBody.linearVelocity * simulation.deltaTime;
			rigidBody.transform.rotation = integrateRotation(rigidBody.transform.rotation, rigidBody.angularVelocity * simulation.deltaTime);

			shapeIdIndex += 1;
		}
	}

	void integratePositions(PhysicsSimulation2D& simulation) {
		integratePositionsType(simulation, view<ShapeID2D>(&simulation.broadPhase.kinematicShapeIds[0], simulation.broadPhase.usedKinematicShapeIdCount));
		integratePositionsType(simulation, view<ShapeID2D>(&simulation.broadPhase.dynamicShapeIds[0], simulation.broadPhase.usedDynamicShapeIdCount));
	}
}