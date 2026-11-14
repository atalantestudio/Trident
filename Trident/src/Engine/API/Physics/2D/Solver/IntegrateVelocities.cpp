// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void integrateVelocities(PhysicsSimulation2D& simulation) {
		uint32 shapeIdIndex = 0;

		// TODO(physics/2d/apply-forces): Integrate velocities on dynamic resting entities (e.g. an object on a moving platform may move due to inertia).
		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Hull2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];
			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + simulation.gravity) * simulation.deltaTime;
			rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * simulation.deltaTime;

			shapeIdIndex += 1;
		}
	}
}