// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void clearPairs(PhysicsSimulation2D& simulation) {
		simulation.collisionPairs.clear();
	}

	void applyForce(PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId, vec2<float32> F) {
		RigidBody2D& rigidBody = simulation.rigidBodies[rigidBodyId];
		rigidBody.force += F;
	}

	void applyImpulse(RigidBody2D& rigidBody, vec2<float32> j, vec2<float32> r) {
		rigidBody.linearVelocity += j * rigidBody.inverseMass;

		// Subtract to get a clockwise rotation.
		rigidBody.angularVelocity -= parallelogramArea(r, j) * rigidBody.inverseInertia;
	}

	mat3x3<float32> computeWorld(const PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId) {
		const RigidBody2D& rigidBody = simulation.rigidBodies[rigidBodyId];

		return {
			translation<mat3x3<float32>>(-rigidBody.centroid) *
			rotationAngle<mat3x3<float32>>(rigidBody.rotation) *
			translation<mat3x3<float32>>(rigidBody.position)
		};
	}
}