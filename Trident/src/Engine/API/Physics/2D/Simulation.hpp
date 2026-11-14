// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/CollisionPair.hpp"
#include "Engine/API/Physics/2D/ContactPair.hpp"
#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION
	#define ENABLE_PHYSICS_2D_WARM_STARTING

	struct PhysicsSimulationDescriptor2D {
		float64 deltaTime;

		/// Left-handed gravity acceleration.
		vec2<float32> gravity;

		float32 allowedPenetration;

		/// Number of iterations in the sequential solver.
		uint32 impulseIterationCount;

		const Scene2D* scene;
	};

	struct BroadPhase2D {
		sequence<ShapeID2D> staticShapeIds;
		sequence<ShapeID2D> kinematicShapeIds;
		sequence<ShapeID2D> dynamicShapeIds;

		uint32 usedStaticShapeIdCount;
		uint32 usedKinematicShapeIdCount;
		uint32 usedDynamicShapeIdCount;
	};

	/// A simulated 2D physics world.
	struct PhysicsSimulation2D {
		float32 deltaTime;
		float32 inverseDeltaTime;
		vec2<float32> gravity;
		float32 allowedPenetration;
		uint32 impulseIterationCount;

		// Index of the current tick since world creation.
		uint32 tick;

		sequence<RigidBody2D> rigidBodies;
		sequence<Hull2D> shapes;

		uint32 usedRigidBodyCount;
		uint32 usedShapeCount;

		BroadPhase2D broadPhase;

		std::vector<CollisionPair2D> collisionPairs;
		std::map<CollisionPair2D, ContactPair2D> contactPairs;
	};

	PhysicsSimulation2D createSimulation(const PhysicsSimulationDescriptor2D& descriptor);

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBody2DComponent& descriptor);

	ShapeID2D createShape(PhysicsSimulation2D& simulation, const Hull2D& descriptor);

	void broadPhase(PhysicsSimulation2D& simulation);
	void narrowPhase(PhysicsSimulation2D& simulation);
	void integrateVelocities(PhysicsSimulation2D& simulation);
	void updateContactManifolds(PhysicsSimulation2D& simulation);
	// void updateJoints(PhysicsSimulation2D& simulation);
	void applyImpulses(PhysicsSimulation2D& simulation);
	void integratePositions(PhysicsSimulation2D& simulation);

	void tick(PhysicsSimulation2D& simulation);
}