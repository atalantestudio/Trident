// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/ContactPair2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/EntityPair.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION
	#define ENABLE_PHYSICS_2D_WARM_STARTING

	struct PhysicsSimulation2DDescriptor {
		Scene2D* scene;

		float64 deltaTime;

		/// Left-handed.
		vec2<float32> gravity;

		// TODO(physics/2d): Ensure that x > 0.
		uint32 maxDynamicHullCount;

		// TODO(physics/2d): Ensure that x > 0.
		uint32 maxDynamicRigidBodyCount;
	};

	static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
	static constexpr float32 ALLOWED_PENETRATION = 0.01f;

	/// A simulated 2D physics world.
	struct PhysicsSimulation2D {
		Scene2D* scene;
		float32 deltaTime;
		float32 inverseDeltaTime;
		vec2<float32> gravity;

		// TODO(physics/2d/delta-time): Need half delta-time too?

		std::vector<EntityPair> entityPairs;
		std::map<EntityPair, ContactPair2D> contactPairs;
	};

	PhysicsSimulation2D createSimulation(const PhysicsSimulation2DDescriptor& descriptor);

	void clearPairs(PhysicsSimulation2D& simulation);
	void broadPhase(PhysicsSimulation2D& simulation);
	void narrowPhase(PhysicsSimulation2D& simulation);
	void applyForces(PhysicsSimulation2D& simulation);
	void updateContactManifolds(PhysicsSimulation2D& simulation);
	void updateJoints(PhysicsSimulation2D& simulation);
	void applyImpulses(PhysicsSimulation2D& simulation);

	/// Uses Semi-Implicit Euler.
	void integrate(PhysicsSimulation2D& simulation);

	const Hull2D& getHull(const PhysicsSimulation2D& simulation, Entity entity);
}