// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void integrateVelocities(PhysicsSimulation2D& simulation) {
		uint32 shapeIdIndex = 0;

		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndex += 1;

				continue;
			}

			rigidBody.linearVelocity += (rigidBody.force * rigidBody.inverseMass + simulation.gravity) * simulation.deltaTime;
			rigidBody.angularVelocity += rigidBody.torque * rigidBody.inverseInertia * simulation.deltaTime;

			shapeIdIndex += 1;
		}
	}
}