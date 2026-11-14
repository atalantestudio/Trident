// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void integratePositions(PhysicsSimulation2D& simulation) {
		uint32 shapeIdIndex = 0;

		// TODO(physics/2d/integration): Integration for static entities (e.g. a moving platform that isn't affected by other forces)?
		// TODO(physics/2d/integration): Integration for dynamic entities with infinite mass?
		// TODO(physics/2d/integration): Integration for dynamic resting entities (e.g. an object on a moving platform may move due to inertia)?
		while (shapeIdIndex < simulation.broadPhase.usedDynamicShapeIdCount) {
			const Hull2D& shape = simulation.shapes[simulation.broadPhase.dynamicShapeIds[shapeIdIndex]];

			RigidBody2D& rigidBody = simulation.rigidBodies[shape.rigidBodyId];
			rigidBody.position += rigidBody.linearVelocity * simulation.deltaTime;
			rigidBody.rotation += rigidBody.angularVelocity * simulation.deltaTime;
			rigidBody.force = {0.0f, 0.0f};
			rigidBody.torque = 0.0f;

			shapeIdIndex += 1;
		}
	}
}