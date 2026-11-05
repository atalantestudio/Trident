// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Base.hpp"
#include "Engine/API/Physics/2D/CollisionPair.hpp"
#include "Engine/API/Physics/2D/ContactPair2D.hpp"
#include "Engine/API/Physics/2D/Shape2D.hpp"
#include "Engine/API/Physics/2D/RigidBody.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	#define ENABLE_PHYSICS_2D_IMPULSE_ACCUMULATION
	#define ENABLE_PHYSICS_2D_WARM_STARTING

	struct PhysicsSimulation2DDescriptor {
		Scene2D* scene;

		float64 deltaTime;

		/// Left-handed.
		vec2<float32> gravity;

		// TODO(physics/2d): Ensure that x > 0.
		uint32 maxStaticRigidBodyCount;
		uint32 maxKinematicAndDynamicRigidBodyCount;
		uint32 maxStaticShapeCount;
		uint32 maxKinematicAndDynamicShapeCount;
	};

	static constexpr uint32 SEQUENTIAL_IMPULSE_ITERATION_COUNT = 10;
	static constexpr float32 ALLOWED_PENETRATION = 0.01f;

	struct BroadPhase2D {
		sequence<uint32> staticShapeIndices;
		sequence<uint32> kinematicShapeIndices;
		sequence<uint32> dynamicShapeIndices;
	};

	/// A simulated 2D physics world.
	struct PhysicsSimulation2D {
		Scene2D* scene;
		float32 deltaTime;
		float32 inverseDeltaTime;
		vec2<float32> gravity;

		// TODO(physics/2d/delta-time): Need half delta-time too?

		sequence<RigidBody2D> rigidBodies;
		sequence<Shape2D> shapes;

		uint32 usedRigidBodyCount;
		uint32 usedShapeCount;

		BroadPhase2D broadPhase;

		std::vector<CollisionPair2D> collisionPairs;
		std::map<CollisionPair2D, ContactPair2D> contactPairs;
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

	RigidBodyID2D createRigidBody(PhysicsSimulation2D& simulation, const RigidBodyDescriptor2D& descriptor);
	ShapeID2D createShape(PhysicsSimulation2D& simulation, const ShapeDescriptor2D& descriptor);

	/// Applies a linear force to the body.
	void applyForce(PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId, vec2<float32> F);

	/// Applies linear and angular impulses to the body.
	void applyImpulse(
		RigidBody2D& rigidBody,

		/// The impulse vector.
		vec2<float32> j,

		/// The vector from the body's position to the contact.
		vec2<float32> r
	);

	mat3x3<float32> computeWorld(const PhysicsSimulation2D& simulation, RigidBodyID2D rigidBodyId);
}