// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	static void integrateType(PhysicsSimulation2D& simulation, view<ShapeID2D> shapeIds) {
		uint32 shapeIdIndex = 0;

		while (shapeIdIndex < shapeIds.count()) {
			const Shape2D& shape = simulation.shapes[shapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndex += 1;

				continue;
			}

			rigidBody.position += rigidBody.state.linearVelocity * simulation.deltaTime;
			rigidBody.rotation += rigidBody.state.angularVelocity * simulation.deltaTime;

			// TODO(physics/2d/integration)
			// rigidBody.state.linearVelocity += (rigidBody.force * rigidBody.inverseMass + simulation.gravity) * simulation.halfDeltaTime;
			// rigidBody.state.angularVelocity += rigidBody.inverseInertia * simulation.halfDeltaTime;

			rigidBody.force = {0.0_N, 0.0_N};

			shapeIdIndex += 1;
		}
	}

	void integrate(PhysicsSimulation2D& simulation) {
		integrateType(simulation, view<ShapeID2D>(&simulation.broadPhase.kinematicShapeIds[0], simulation.broadPhase.usedKinematicShapeIdCount));
		integrateType(simulation, view<ShapeID2D>(&simulation.broadPhase.dynamicShapeIds[0], simulation.broadPhase.usedDynamicShapeIdCount));
	}
}