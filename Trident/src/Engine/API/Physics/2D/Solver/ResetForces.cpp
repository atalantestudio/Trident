// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void resetForcesType(PhysicsSimulation2D& simulation, view<ShapeID2D> shapeIds) {
		uint32 shapeIdIndex = 0;

		while (shapeIdIndex < shapeIds.count()) {
			const Shape2D& shape = simulation.shapes[shapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndex += 1;

				continue;
			}

			rigidBody.force = {0.0f, 0.0f};
			rigidBody.torque = 0.0f;

			shapeIdIndex += 1;
		}
	}

	void resetForces(PhysicsSimulation2D& simulation) {
		resetForcesType(simulation, view<ShapeID2D>(&simulation.broadPhase.kinematicShapeIds[0], simulation.broadPhase.usedKinematicShapeIdCount));
		resetForcesType(simulation, view<ShapeID2D>(&simulation.broadPhase.dynamicShapeIds[0], simulation.broadPhase.usedDynamicShapeIdCount));
	}
}