// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/CollisionPair.hpp"
#include "Engine/API/Physics/2D/ContactPair.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION
	#define ENABLE_PHYSICS_2D_WARM_STARTING

	struct PhysicsSimulationDescriptor2D {
		Scene2D* scene;
		float64 deltaTime;

		/// Left-handed gravity acceleration.
		vec2<float32> gravity;

		/// Number of iterations in the sequential solver.
		uint32 impulseIterationCount;

		float32 allowedPenetration;
	};

	/// A simulated 2D physics world.
	struct PhysicsSimulation2D {
		float32 deltaTime;
		float32 inverseDeltaTime;
		vec2<float32> gravity;

		// Index of the current tick since world creation.
		uint32 tick;

		uint32 impulseIterationCount;
		float32 allowedPenetration;

		Scene2D* scene;

		std::vector<CollisionPair2D> collisionPairs;
		std::map<CollisionPair2D, ContactPair2D> contactPairs;
	};

	PhysicsSimulation2D createSimulation(const PhysicsSimulationDescriptor2D& descriptor);

	void tick(PhysicsSimulation2D& simulation);

	void broadPhase(PhysicsSimulation2D& simulation);
	void narrowPhase(PhysicsSimulation2D& simulation);
	void integrateVelocities(PhysicsSimulation2D& simulation);
	void updateContactManifolds(PhysicsSimulation2D& simulation);
	// void updateJoints(PhysicsSimulation2D& simulation);
	void applyImpulses(PhysicsSimulation2D& simulation);
	void integratePositions(PhysicsSimulation2D& simulation);
}