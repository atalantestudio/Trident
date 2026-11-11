// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	void clearPairs(PhysicsSimulation2D& simulation) {
		simulation.collisionPairs.clear();
	}

	void applyImpulse(RigidBody2D& rigidBody, vec2<float32> j, vec2<float32> r) {
		rigidBody.linearVelocity += j * rigidBody.inverseMass;
		rigidBody.angularVelocity += parallelogramArea(r, j) * rigidBody.inverseInertia;
	}
}