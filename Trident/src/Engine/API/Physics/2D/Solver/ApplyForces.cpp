// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void applyForces(PhysicsSimulation2D& simulation) {
		// TODO(physics/2d/apply-forces): Implement torque.
		constexpr float32 torque = 0.0_Nm;

		uint32 shapeIdIndex = 0;

		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Shape2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];

			if (rigidBody.flags & RigidBodyFlags2D::DISABLE_SIMULATION) {
				shapeIdIndex += 1;

				continue;
			}

			// TODO(physics/2d/apply-forces)
			// Integrate gravity like in https://web.archive.org/web/20161201054257/https://www.niksula.hut.fi/~hkankaan/Homepages/gravity.html.
			// This looks like Velocity Verlet for gravity to me?
			// rigidBody.state.linearVelocity += (rigidBody.force * rigidBody.inverseMass + simulation.gravity) * simulation.halfDeltaTime;
			rigidBody.state.linearVelocity += ((rigidBody.force + simulation.gravity) * rigidBody.inverseMass) * simulation.deltaTime;
			// rigidBody.state.angularVelocity += rigidBody.inverseInertia * halfDeltaTime;
			rigidBody.state.angularVelocity += torque * rigidBody.inverseInertia * simulation.deltaTime;

			shapeIdIndex += 1;
		}
	}
}